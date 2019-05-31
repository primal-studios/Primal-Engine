#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanVertexBuffer.h"

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
		case EBufferLayoutElementTypes::VEC2: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::VEC3: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::VEC4: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::MAT2: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::MAT3: return VK_FORMAT_R32_SFLOAT;
		case EBufferLayoutElementTypes::MAT4: return VK_FORMAT_R32_SFLOAT;
		default: return VkFormat(0);
	}
}

VulkanVertexBuffer::VulkanVertexBuffer(IGraphicsContext* aContext) : IVertexBuffer(aContext)
{
	mBuffer = nullptr;
	mStagingBuffer = nullptr;

	mBindingDescription = {};

	mSize = 0;
	mData = nullptr;
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	vmaDestroyBuffer(context->getBufferAllocator(), mBuffer, mAllocation);
	vmaFreeMemory(context->getBufferAllocator(), mAllocation);

	mBuffer = nullptr;

	free(mData);
}

void VulkanVertexBuffer::construct(const VertexBufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);

	const bool usesStaging = (aInfo.usage & EBufferUsageFlags::BUFFER_USAGE_TRANSFER_DST) != 0;
	const bool isExclusive = (aInfo.sharingMode & ESharingMode::SHARING_MODE_EXCLUSIVE) != 0;

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
		
		void* data;
		vmaMapMemory(context->getBufferAllocator(), mAllocation, &data);
		memcpy(data, mData, mSize);
		vmaUnmapMemory(context->getBufferAllocator(), mAllocation);

		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = mSize;
		createInfo.usage = aInfo.usage;
		createInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, nullptr);

		// Copy buffers
		const auto vulkanCommandPool = static_cast<VulkanCommandPool*>(aInfo.commandPool);

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

		vkQueueSubmit(reinterpret_cast<VkQueue>(static_cast<uint64_t>(context->getGraphicsQueueIndex())), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(reinterpret_cast<VkQueue>(static_cast<uint64_t>(context->getGraphicsQueueIndex())));
		vkFreeCommandBuffers(context->getDevice(), vulkanCommandPool->getPool(), 1, &commandBuffer);

		vmaDestroyBuffer(context->getBufferAllocator(), mStagingBuffer, mStagingAllocation);
		vmaFreeMemory(context->getBufferAllocator(), mStagingAllocation);
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

		void* data;
		vmaMapMemory(context->getBufferAllocator(), mAllocation, &data);
		memcpy(data, mData, mSize);
		vmaUnmapMemory(context->getBufferAllocator(), mAllocation);
	}
}

void VulkanVertexBuffer::setData(void* aData, const size_t aSize)
{
	mSize = aSize;
	memcpy(mData, aData, aSize);
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

