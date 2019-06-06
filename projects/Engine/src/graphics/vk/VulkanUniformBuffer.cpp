#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanSampler.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanDescriptorSets.h"

VulkanUniformBuffer::VulkanUniformBuffer(IGraphicsContext* aContext) 
	: IUniformBuffer(aContext)
{
	mLayout = nullptr;
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	_destroy();
}

void VulkanUniformBuffer::construct(const UniformBufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	mInfo = aInfo;

	mLayout = new VulkanDescriptorSetLayout(mContext);
	mLayout->construct({ 0, {{aInfo.binding, EDescriptorType::UNIFORM_BUFFER, aInfo.shaderStageFlags, aInfo.immutableSamplers}} });

	mSize = static_cast<VkDeviceSize>(aInfo.size);
	const bool isExclusive = (aInfo.sharingMode == ESharingMode::SHARING_MODE_EXCLUSIVE);

	mBuffer.resize(aInfo.framesInFlight);
	mAllocation.resize(aInfo.framesInFlight);

	for(size_t i = 0; i < aInfo.framesInFlight; i++)
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = mSize;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

		const VkResult res = vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer[i], &mAllocation[i], nullptr);

		if (res != VK_SUCCESS)
		{
			PRIMAL_INTERNAL_CRITICAL("Failed to create uniform buffer.");
		}
		else
		{
			PRIMAL_INTERNAL_INFO("Successfully created uniform buffer.");
		}
	}
}

void VulkanUniformBuffer::reconstruct(const UniformBufferCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

void VulkanUniformBuffer::setData(void* aData, const size_t aSize, const size_t aCurrentImage)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	void* data;
	vmaMapMemory(context->getBufferAllocator(), mAllocation[aCurrentImage], &data);
	memcpy(data, aData, aSize);
	vmaUnmapMemory(context->getBufferAllocator(), mAllocation[aCurrentImage]);
}

VkWriteDescriptorSet VulkanUniformBuffer::getWriteDescriptorSet(const uint32_t aCurrentFrame)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = mBuffer[aCurrentFrame];
	bufferInfo.offset = 0;
	bufferInfo.range = mSize;

	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	VulkanDescriptorSets* set = primal_cast<VulkanDescriptorSets*>(mInfo.descriptorSets);
	descriptorWrite.dstSet = set->getSet(aCurrentFrame);
	descriptorWrite.dstBinding = mInfo.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = mInfo.descriptorCount;
	descriptorWrite.pBufferInfo = &bufferInfo;

	return descriptorWrite;
}

void VulkanUniformBuffer::_destroy()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	for (size_t i = 0; i < mBuffer.size(); i++)
	{
		vmaDestroyBuffer(context->getBufferAllocator(), mBuffer[i], mAllocation[i]);
	}
}
