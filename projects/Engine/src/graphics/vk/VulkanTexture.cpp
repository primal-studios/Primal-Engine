#include "graphics/vk/VulkanTexture.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanDescriptorSet.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanSampler.h"
#include "graphics/vk/VulkanGraphicsContext.h"

#include "assets/TextureAsset.h"

VulkanTexture::VulkanTexture(IGraphicsContext* aContext)
	: ITexture(aContext), mSampler(nullptr)
{
	mImage = nullptr;
	mImageView = nullptr;
}

VulkanTexture::~VulkanTexture()
{
	delete mImageView;
	delete mImage;
}

void VulkanTexture::construct(const TextureCreateInfo& aInfo)
{
	mBindingPoint = aInfo.binding;
	mStages = aInfo.shaderStageAccess;

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
}

DescriptorSetLayoutBinding VulkanTexture::getDescriptorSetLayout(const uint32_t aBinding,
	const VkShaderStageFlags aStage, const uint32_t aCount)
{
	DescriptorSetLayoutBinding binding = {};
	binding.binding = 1;
	binding.descriptorCount = 1;
	binding.descriptorType = EDescriptorType::COMBINED_IMAGE_SAMPLER;
	binding.shaderStageFlags = EShaderStageFlagBits::SHADER_STAGE_FRAGMENT;

	return binding;
}

WriteDescriptorSet VulkanTexture::getWriteDescriptor(const uint32_t aBinding,
	const std::optional<OffsetSize>& aOffsetSize) const
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = mImageView->getHandle();

	VulkanSampler* sampler = primal_cast<VulkanSampler*>(mSampler);
	imageInfo.sampler = sampler->getHandle();

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = nullptr;
	descriptorWrite.dstBinding = aBinding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.pImageInfo = &imageInfo;

	return { descriptorWrite, imageInfo };
}

ShaderStageFlags VulkanTexture::getStageFlags() const
{
	return mStages;
}

uint32_t VulkanTexture::getBindingPoint() const
{
	return mBindingPoint;
}
