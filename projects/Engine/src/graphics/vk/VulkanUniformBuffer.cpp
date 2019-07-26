#include "graphics/vk/VulkanUniformBuffer.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanDescriptorPool.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanDescriptorSet.h"

VulkanUniformBuffer::VulkanUniformBuffer(IGraphicsContext* aContext)
	: IUniformBuffer(aContext), mBuffer(nullptr), mAllocation(nullptr)
{
	mSize = 0;
	mInfo = {};
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	_destroy();
}

void VulkanUniformBuffer::construct(const UniformBufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	mInfo = aInfo;

	mSize = static_cast<VkDeviceSize>(aInfo.size);
	const bool isExclusive = (aInfo.sharingMode == ESharingMode::SHARING_MODE_EXCLUSIVE);

	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = mSize;
	createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	const VkResult res = vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);

	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create uniform buffer.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created uniform buffer.");
	}
}

void VulkanUniformBuffer::reconstruct(const UniformBufferCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

void VulkanUniformBuffer::setData(void* aData, const size_t aOffset, const size_t aSize)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	void* data;
	vmaMapMemory(context->getBufferAllocator(), mAllocation, &data);
	memcpy(static_cast<uint8_t*>(data) + aOffset, aData, aSize);
	vmaUnmapMemory(context->getBufferAllocator(), mAllocation);
}

DescriptorSetLayoutBinding VulkanUniformBuffer::getDescriptorSetLayout(const uint32_t aBinding, const VkShaderStageFlags aStage, const uint32_t aCount)
{
	DescriptorSetLayoutBinding binding = {};
	binding.binding = aBinding;
	binding.descriptorCount = 1;
	binding.descriptorType = EDescriptorType::UNIFORM_BUFFER_DYNAMIC;
	binding.shaderStageFlags = aStage;

	return binding;
}

WriteDescriptorSet VulkanUniformBuffer::getWriteDescriptor(const uint32_t aBinding, const std::optional<OffsetSize>& aOffsetSize, bool aIsDynamic) const
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = mBuffer;
	bufferInfo.offset = 0;
	bufferInfo.range = mInfo.size;

	if (aOffsetSize)
	{
		bufferInfo.offset = aOffsetSize->getOffset();
		bufferInfo.range = aOffsetSize->getSize();
	}

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = nullptr;
	descriptorWrite.dstBinding = aBinding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType = aIsDynamic ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return { descriptorWrite, bufferInfo };
}

void VulkanUniformBuffer::_destroy() const
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vmaDestroyBuffer(context->getBufferAllocator(), mBuffer, mAllocation);
}
