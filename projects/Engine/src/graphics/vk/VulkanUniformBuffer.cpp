#include "graphics/vk/VulkanUniformBuffer.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanDescriptorPool.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanDescriptorSet.h"

VulkanUniformBuffer::VulkanUniformBuffer(IGraphicsContext* aContext)
	: IUniformBuffer(aContext), mLayout(nullptr), mSets(nullptr)
{
	mSize = 0;
	mInfo = {};
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	_destroy();
	delete mLayout;
}

void VulkanUniformBuffer::construct(const UniformBufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	mInfo = aInfo;

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

	// Construct sets
	if (!mLayout)
	{
		DescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.flags = 0;
		createInfo.layoutBindings = { aInfo.binding };

		mLayout = new VulkanDescriptorSetLayout(mContext);
		mLayout->construct(createInfo);
	}

	if (!mSets)
	{
		const std::vector<IDescriptorSetLayout*> layouts(aInfo.framesInFlight, mLayout);

		DescriptorSetCreateInfo allocInfo = {};
		allocInfo.setLayouts = layouts;
		allocInfo.pool = aInfo.descriptorPool;

		mSets = new VulkanDescriptorSet(mContext);
		mSets->construct(allocInfo);
	}

	VulkanDescriptorSet* vulkanSets = primal_cast<VulkanDescriptorSet*>(mSets);
	for(uint32_t i = 0; i < aInfo.framesInFlight; i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mBuffer[i];
		bufferInfo.offset = 0;
		bufferInfo.range = mSize;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = vulkanSets->getHandle(i);
		descriptorWrite.dstBinding = aInfo.binding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(context->getDevice(), 1, &descriptorWrite, 0, nullptr);
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

IDescriptorSetLayout* VulkanUniformBuffer::getLayout() const
{
	return mLayout;
}

IDescriptorSet* VulkanUniformBuffer::getSet() const
{
	return mSets;
}

void VulkanUniformBuffer::_destroy()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	for (size_t i = 0; i < mBuffer.size(); i++)
	{
		vmaDestroyBuffer(context->getBufferAllocator(), mBuffer[i], mAllocation[i]);
	}
}
