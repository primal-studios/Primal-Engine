#include "graphics/vk/VulkanTexture.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanDescriptorSet.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanSampler.h"
#include "graphics/vk/VulkanGraphicsContext.h"

VulkanTexture::VulkanTexture(IGraphicsContext* aContext)
	: ITexture(aContext)
{
	mImage = nullptr;
	mImageView = nullptr;
	mSets = nullptr;
	mLayout = nullptr;
}

VulkanTexture::~VulkanTexture()
{
	delete mSets;
	delete mLayout;
	delete mImageView;
	delete mImage;
	delete mSampler;
}

void VulkanTexture::construct(const TextureCreateInfo& aInfo)
{
	mImage = new VulkanImage(mContext);
	const uint32_t size = aInfo.textureAsset->getData().channels * (aInfo.textureAsset->getData().height * aInfo.textureAsset->getData().width);
	mImage->setData(&aInfo.textureAsset->getData().payload[0], size);
	mImage->construct({ 0, EImageDimension::IMAGE_2D, EDataFormat::R8G8B8A8_UNORM, aInfo.textureAsset->getData().width,
	aInfo.textureAsset->getData().height, 1, 0, 1, 0, 1, 1, IMAGE_ASPECT_COLOR,
	IMAGE_SAMPLE_1, EImageTiling::IMAGE_TILING_OPTIMAL, IMAGE_USAGE_TRANSFER_DST_BIT | IMAGE_USAGE_SAMPLED_BIT,
	IMAGE_LAYOUT_UNDEFINED, {mContext}, EMemoryPropertyBits::MEMORY_PROPERTY_DEVICE_LOCAL, 0, 0 });

	mSampler = primal_cast<VulkanSampler*>(aInfo.sampler);

	mImageView = new VulkanImageView(mContext);
	mImageView->construct({ mImage, EDataFormat::R8G8B8A8_UNORM, EImageViewType::IMAGE_VIEW_TYPE_2D, {EImageAspectFlagBits::IMAGE_ASPECT_COLOR,
	0, 1, 0, 1} });

	mLayout = new VulkanDescriptorSetLayout(mContext);
	DescriptorSetLayoutBinding binding = {};
	binding.binding = 1;
	binding.descriptorCount = 1;
	binding.descriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER;
	binding.shaderStageFlags = EShaderStageFlagBits::SHADER_STAGE_FRAGMENT;

	mLayout->construct({ 0, {binding} });

	if (!mSets)
	{
		const std::vector<IDescriptorSetLayout*> layouts(aInfo.framesInFlight, mLayout);

		DescriptorSetCreateInfo allocInfo = {};
		allocInfo.setLayouts = layouts;
		allocInfo.pool = aInfo.descriptorPool;

		mSets = new VulkanDescriptorSet(mContext);
		mSets->construct(allocInfo);
	}

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VulkanDescriptorSet* vulkanSets = primal_cast<VulkanDescriptorSet*>(mSets);
	for (uint32_t i = 0; i < aInfo.framesInFlight; i++)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = mImageView->getHandle();
		
		VulkanSampler* sampler = primal_cast<VulkanSampler*>(aInfo.sampler);
		imageInfo.sampler = sampler->getHandle();

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = vulkanSets->getHandle(i);
		descriptorWrite.dstBinding = 1;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pBufferInfo = nullptr;

		vkUpdateDescriptorSets(context->getDevice(), 1, &descriptorWrite, 0, nullptr);
	}
}

void VulkanTexture::bind(const uint32_t aBinding)
{

}

IDescriptorSetLayout* VulkanTexture::getLayout() const
{
	return mLayout;
}

IDescriptorSet* VulkanTexture::getSet() const
{
	return mSets;
}
