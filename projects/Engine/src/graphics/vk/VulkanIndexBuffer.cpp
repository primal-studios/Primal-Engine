#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "core/PrimalCast.h"

VulkanIndexBuffer::VulkanIndexBuffer(IGraphicsContext* aContext) : IIndexBuffer(aContext)
{
	mBuffer = nullptr;
	mStagingBuffer = nullptr;

	mSize = 0;
	mData = nullptr;
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vmaDestroyBuffer(context->getBufferAllocator(), mBuffer, mAllocation);

	mBuffer = nullptr;

	free(mData);
}

void VulkanIndexBuffer::construct(const IndexBufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	const bool usesStaging = (aInfo.usage & BUFFER_USAGE_TRANSFER_DST) != 0;
	const bool isExclusive = (aInfo.sharingMode == ESharingMode::SHARING_MODE_EXCLUSIVE);

	if (usesStaging)
	{
		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = mSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo stagingAllocInfo = {};
		stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &stagingBufferInfo, &stagingAllocInfo, &mStagingBuffer, &mStagingAllocation, nullptr);

		void* data = malloc(mSize);
		vmaMapMemory(context->getBufferAllocator(), mStagingAllocation, &data);
		memcpy(data, mData, mSize);
		vmaUnmapMemory(context->getBufferAllocator(), mStagingAllocation);

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = mSize;
		createInfo.usage = aInfo.usage;
		createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);

		// Copy buffers
		const auto vulkanCommandPool = context->getCommandPool();

		VkCommandBufferAllocateInfo copyAllocInfo = {};
		copyAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		copyAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		copyAllocInfo.commandPool = vulkanCommandPool->getPool();
		copyAllocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(context->getDevice(), &copyAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = mSize;
		vkCmdCopyBuffer(commandBuffer, mStagingBuffer, mBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		VkQueue graphicsQueue;
		vkGetDeviceQueue(context->getDevice(), context->getPresentQueueIndex(), 0, &graphicsQueue);
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, nullptr);
		vkQueueWaitIdle(graphicsQueue);
		vkFreeCommandBuffers(context->getDevice(), vulkanCommandPool->getPool(), 1, &commandBuffer);

		vmaDestroyBuffer(context->getBufferAllocator(), mStagingBuffer, mStagingAllocation);
	}
	else
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = mSize;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);

		void* data = malloc(mSize);
		vmaMapMemory(context->getBufferAllocator(), mAllocation, &data);
		memcpy(data, mData, mSize);
		vmaUnmapMemory(context->getBufferAllocator(), mAllocation);
	}
}

void VulkanIndexBuffer::setData(void* aData, const size_t aSize)
{
	mSize = aSize;
	mData = malloc(mSize);
	if (mData)
	{
		memcpy(mData, aData, aSize);
	}
}

void VulkanIndexBuffer::bind()
{
	// TODO: Implement
}

void VulkanIndexBuffer::unbind()
{
	// TODO: Implement
}

VkBuffer VulkanIndexBuffer::getHandle() const
{
	return mBuffer;
}

