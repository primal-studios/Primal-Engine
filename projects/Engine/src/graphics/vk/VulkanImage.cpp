#include "core/Log.h"
#include "graphics/vk/VulkanImage.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "core/PrimalAssert.h"
#include "graphics/api/ICommandBuffer.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "core/PrimalCast.h"

VulkanImage::VulkanImage(IGraphicsContext* aContext)
	: IImage(aContext), mContext(aContext)
{
	VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(aContext);
	mAllocator = ctx->getImageAllocator();
}

VulkanImage::~VulkanImage()
{
	_destroy();
}

void VulkanImage::construct(const ImageCreateInfo& aInfo)
{
	using namespace std;

	if (mStagingMemory)
	{
		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = mStagingSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(mContext);

		VmaAllocationCreateInfo stagingAllocInfo = {};
		stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		vmaCreateBuffer(ctx->getBufferAllocator(), &stagingBufferInfo, &stagingAllocInfo, &mStagingBuffer, &mStagingAllocation, nullptr);
		void* data;
		vmaMapMemory(ctx->getBufferAllocator(), mStagingAllocation, &data);
		memcpy(data, mStagingMemory, mStagingSize);
		vmaUnmapMemory(ctx->getBufferAllocator(), mStagingAllocation);
	}

	VkImageCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = aInfo.flags;
	createInfo.imageType = static_cast<VkImageType>(aInfo.dimension);
	createInfo.format = static_cast<VkFormat>(aInfo.format);
	createInfo.extent = { aInfo.width, aInfo.height, aInfo.depth };
	createInfo.mipLevels = aInfo.mipLayerCount;
	createInfo.arrayLayers = aInfo.layerCount;
	createInfo.samples = static_cast<VkSampleCountFlagBits>(aInfo.samples);
	createInfo.tiling = static_cast<VkImageTiling>(aInfo.tiling);
	createInfo.usage = aInfo.usage;
	createInfo.initialLayout = static_cast<VkImageLayout>(aInfo.layout);

	vector<uint32_t> queueFamilies;

	if (aInfo.contexts.size() <= 1)
	{
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else
	{
		createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	}

	for (auto context : aInfo.contexts)
	{
		VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(context);
		uint32_t q = ctx->getGraphicsQueueIndex();
		queueFamilies.push_back(q);
	}

	createInfo.pQueueFamilyIndices = queueFamilies.data();
	createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = aInfo.memoryRequiredFlags;
	allocInfo.preferredFlags = aInfo.memoryPreferredFlags;
	allocInfo.flags = aInfo.memoryAllocationFlags;

	const VkResult res = vmaCreateImage(mAllocator, &createInfo, &allocInfo, &mImage, &mAllocation, nullptr);

	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan image.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan image.");
	}

	if (mStagingMemory != nullptr)
	{
		transitionToLayout(aInfo, aInfo.format, IMAGE_LAYOUT_UNDEFINED, IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		_copyBufferToImage(aInfo);
		transitionToLayout(aInfo, aInfo.format, IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		
		VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(mContext);
		vmaDestroyBuffer(ctx->getBufferAllocator(), mStagingBuffer, mStagingAllocation);
		free(mStagingMemory);
	}
}

void VulkanImage::reconstruct(const ImageCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

VkImage VulkanImage::getHandle() const
{
	return mImage;
}

void VulkanImage::setHandle(const VkImage aImage)
{
	mOwning = false;
	mImage = aImage;
}

void VulkanImage::setData(void* aData, const size_t aSize)
{
	if (mStagingMemory)
	{
		free(mStagingMemory);
		mStagingMemory = nullptr;
	}

	mStagingSize = aSize;
	mStagingMemory = malloc(aSize);
	PRIMAL_ASSERT(mStagingMemory != nullptr, "Failed to allocate staging memory.");
}

void VulkanImage::transitionToLayout(const ImageCreateInfo& aInfo, EDataFormat aFormat, EImageLayout aOldLayout, EImageLayout aNewLayout) const
{
	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = static_cast<VkImageLayout>(aOldLayout);
	barrier.newLayout = static_cast<VkImageLayout>(aNewLayout);
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = mImage;
	barrier.subresourceRange.aspectMask = aInfo.imageAspect;
	barrier.subresourceRange.baseMipLevel = aInfo.baseMipLevel;
	barrier.subresourceRange.levelCount = aInfo.levelCount;
	barrier.subresourceRange.layerCount = aInfo.layerCount;
	barrier.subresourceRange.baseArrayLayer = aInfo.baseArrayLayer;

	VkPipelineStageFlags sourceStage = 0;
	VkPipelineStageFlags destinationStage = 0;

	const VkImageLayout oldLayout = static_cast<VkImageLayout>(aOldLayout);
	const VkImageLayout newLayout = static_cast<VkImageLayout>(aNewLayout);

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else
	{
		PRIMAL_ASSERT(false, "Unsupported transition.");
	}

	VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	VulkanCommandPool* pool = ctx->getCommandPool();

	const CommandBufferCreateInfo commandBufferInfo = {
		pool,
		true
	};

	VulkanCommandBuffer buf(ctx);
	buf.construct(commandBufferInfo);
	const VkCommandBuffer bufHandle = buf.getHandle();

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(bufHandle, &beginInfo);
	vkCmdPipelineBarrier(bufHandle, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	vkEndCommandBuffer(bufHandle);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &bufHandle;

	const uint32_t queueIdx = ctx->getGraphicsQueueIndex();
	VkQueue queue;

	vkGetDeviceQueue(ctx->getDevice(), queueIdx, 0, &queue);

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
}

void VulkanImage::_copyBufferToImage(const ImageCreateInfo& aInfo) const
{
	VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	VulkanCommandPool* pool = ctx->getCommandPool();

	const CommandBufferCreateInfo commandBufferInfo = {
		pool,
		true
	};

	VulkanCommandBuffer buf(ctx);
	buf.construct(commandBufferInfo);
	const VkCommandBuffer bufHandle = buf.getHandle();

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VkBufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = aInfo.imageAspect;
	region.imageSubresource.mipLevel = aInfo.baseMipLevel;
	region.imageSubresource.baseArrayLayer = aInfo.baseArrayLayer;
	region.imageSubresource.layerCount = aInfo.layerCount;
	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = { aInfo.width, aInfo.height, aInfo.depth };

	vkBeginCommandBuffer(bufHandle, &beginInfo);
	vkCmdCopyBufferToImage(bufHandle, mStagingBuffer, mImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	vkEndCommandBuffer(bufHandle);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &bufHandle;

	const uint32_t queueIdx = ctx->getGraphicsQueueIndex();
	VkQueue queue;

	vkGetDeviceQueue(ctx->getDevice(), queueIdx, 0, &queue);

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);
}

void VulkanImage::_destroy() const
{
	if (mOwning)
	{
		vmaDestroyImage(mAllocator, mImage, mAllocation);
	}
}
