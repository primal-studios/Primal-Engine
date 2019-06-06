#include "core/Log.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanImage.h"
#include "graphics/vk/VulkanImageView.h"
#include "graphics/vk/VulkanSampler.h"
#include "graphics/vk/VulkanDescriptorSets.h"
#include "core/PrimalCast.h"

VulkanImageView::VulkanImageView(IGraphicsContext* aContext)
	: IImageView(aContext), mContext(aContext)
{
}

VulkanImageView::~VulkanImageView()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyImageView(context->getDevice(), mImageView, nullptr);
}

void VulkanImageView::construct(const ImageViewCreateInfo& aInfo)
{
	VkImageViewCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.pNext = nullptr;
	info.image = reinterpret_cast<VulkanImage*>(aInfo.image)->getHandle();
	info.format = static_cast<VkFormat>(aInfo.format);
	info.viewType = static_cast<VkImageViewType>(aInfo.viewType);
	info.subresourceRange.aspectMask = aInfo.subresourceRange.aspect;
	info.subresourceRange.baseArrayLayer = aInfo.subresourceRange.baseArrayLayer;
	info.subresourceRange.baseMipLevel = aInfo.subresourceRange.baseMipLevel;
	info.subresourceRange.layerCount = aInfo.subresourceRange.layerCount;
	info.subresourceRange.levelCount = aInfo.subresourceRange.levelCount;

	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);

	const VkResult res = vkCreateImageView(context->getDevice(), &info, nullptr, &mImageView);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan image view.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan image view.");
	}
}

VkImageView VulkanImageView::getHandle() const
{
	return mImageView;
}

VkWriteDescriptorSet VulkanImageView::getWriteDescriptorSet(const uint32_t aCurrentFrame, ISampler* aSampler, IDescriptorSets* aSets) const
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = getHandle();
	VulkanSampler* vSampler = static_cast<VulkanSampler*>(aSampler);

	imageInfo.sampler = vSampler->getHandle();

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	VulkanDescriptorSets* set = primal_cast<VulkanDescriptorSets*>(aSets);
	descriptorWrite.dstSet = set->getSet(aCurrentFrame);
	descriptorWrite.dstBinding = mInfo.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = mInfo.descriptorCount;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return descriptorWrite;
}
