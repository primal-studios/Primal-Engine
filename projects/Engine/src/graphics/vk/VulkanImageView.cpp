#include <core/Log.h>
#include <graphics/vk/VulkanGraphicsContext.h>
#include <graphics/vk/VulkanImage.h>
#include <graphics/vk/VulkanImageView.h>

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
