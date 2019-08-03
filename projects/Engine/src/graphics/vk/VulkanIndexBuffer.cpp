#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "core/PrimalCast.h"

VulkanIndexBuffer::VulkanIndexBuffer(IGraphicsContext* aContext) 
	: IIndexBuffer(aContext), mInfo({})
{
	mBuffer = nullptr;
	mStagingBuffer = nullptr;
}

VulkanIndexBuffer::~VulkanIndexBuffer()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vmaDestroyBuffer(context->getBufferAllocator(), mBuffer, mAllocation);

	mBuffer = nullptr;
}

void VulkanIndexBuffer::construct(const IndexBufferCreateInfo& aInfo)
{
	mInfo = aInfo;

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	const bool usesStaging = (aInfo.usage & BUFFER_USAGE_TRANSFER_DST) != 0;
	const bool isExclusive = (aInfo.sharingMode == ESharingMode::SHARING_MODE_EXCLUSIVE);

	if (usesStaging)
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = aInfo.size;
		createInfo.usage = aInfo.usage;
		createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);
	}
	else
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = aInfo.size;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);
	}
}

uint32_t VulkanIndexBuffer::getCount() const
{
	return mInfo.size >> 1;
}
