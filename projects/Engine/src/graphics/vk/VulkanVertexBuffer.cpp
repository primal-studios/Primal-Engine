#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanVertexBuffer.h"
#include "core/PrimalCast.h"

static constexpr VkFormat sVertexAttributeFormat(const EBufferLayoutElementTypes& aType)
{
	switch(aType)
	{
		case EBufferLayoutElementTypes::UBYTE: return VK_FORMAT_R8_UINT;
		case EBufferLayoutElementTypes::BYTE: return VK_FORMAT_R8_SINT;
		case EBufferLayoutElementTypes::USHORT: return VK_FORMAT_R16_UINT;
		case EBufferLayoutElementTypes::SHORT: return VK_FORMAT_R16_SINT;
		case EBufferLayoutElementTypes::UINT: return VK_FORMAT_R32_UINT;
		case EBufferLayoutElementTypes::INT: return VK_FORMAT_R32_SINT;
		case EBufferLayoutElementTypes::ULONG: return VK_FORMAT_R64_UINT;
		case EBufferLayoutElementTypes::LONG: return VK_FORMAT_R64_SINT;
		case EBufferLayoutElementTypes::FLOAT: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::DOUBLE: return VK_FORMAT_R64_SFLOAT;
		case EBufferLayoutElementTypes::VEC2: return VK_FORMAT_R32G32_SFLOAT;
		case EBufferLayoutElementTypes::VEC3: return VK_FORMAT_R32G32B32_SFLOAT;
		case EBufferLayoutElementTypes::VEC4: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case EBufferLayoutElementTypes::MAT2: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::MAT3: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::MAT4: return VK_FORMAT_R32_SFLOAT;
		default: return VkFormat(0);
	}
}

VulkanVertexBuffer::VulkanVertexBuffer(IGraphicsContext* aContext) 
	: IVertexBuffer(aContext)
{
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vmaDestroyBuffer(context->getBufferAllocator(), mBuffer, mAllocation);

	mBuffer = nullptr;

	free(mData);
}

void VulkanVertexBuffer::construct(const VertexBufferCreateInfo& aInfo)
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

void VulkanVertexBuffer::setData(void* aData, const size_t aSize)
{
	mSize = aSize;
	mData = malloc(aSize);
	if (mData)
	{
		memcpy(mData, aData, aSize);
	}
}

void VulkanVertexBuffer::setLayout(const BufferLayout& aLayout)
{
	mLayout = aLayout;

	mBindingDescription.binding = 0;
	mBindingDescription.stride = static_cast<uint32_t>(aLayout.getStride());
	mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	for(size_t i = 0; i < aLayout.getLayout().size(); i++)
	{
		VkVertexInputAttributeDescription desc = {};
		desc.binding = 0;
		desc.location = static_cast<uint32_t>(i);
		desc.format = sVertexAttributeFormat(aLayout.getLayout()[i].type);
		desc.offset = static_cast<uint32_t>(aLayout.getLayout()[i].offset);

		mAttributeDescriptions.push_back(desc);
	}
}

void VulkanVertexBuffer::bind()
{
	// TODO: Implement
}

void VulkanVertexBuffer::unbind()
{
	// TODO: Implement
}

VkBuffer VulkanVertexBuffer::getHandle() const
{
	return mBuffer;
}

VertexInputBindingDescription VulkanVertexBuffer::getBinding() const
{
	VertexInputBindingDescription desc = {};
	desc.binding = mBindingDescription.binding;
	desc.rate = static_cast<EVertexInputRate>(mBindingDescription.inputRate);
	desc.stride = mBindingDescription.stride;

	return desc;
}

std::vector<VertexInputAttributeDescription> VulkanVertexBuffer::getAttributes() const
{
	std::vector<VertexInputAttributeDescription> attrs;

	for(const auto& attribute : mAttributeDescriptions)
	{
		VertexInputAttributeDescription desc = {};
		desc.binding = attribute.binding;
		desc.format = static_cast<EDataFormat>(attribute.format);
		desc.location = attribute.location;
		desc.offset = attribute.offset;

		attrs.push_back(desc);
	}

	return attrs;
}

