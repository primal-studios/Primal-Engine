#include "graphics/vk/VulkanVertexBuffer.h"

#include "graphics/vk/VulkanGraphicsContext.h"

static VkFormat sVertexAttributeFormat(const EVertexBufferLayoutElementTypes& aType)
{
	switch(aType)
	{
		case EVertexBufferLayoutElementTypes::UBYTE: return VK_FORMAT_R8_UINT;
		case EVertexBufferLayoutElementTypes::BYTE: return VK_FORMAT_R8_SINT;
		case EVertexBufferLayoutElementTypes::USHORT: return VK_FORMAT_R16_UINT;
		case EVertexBufferLayoutElementTypes::SHORT: return VK_FORMAT_R16_SINT;
		case EVertexBufferLayoutElementTypes::UINT: return VK_FORMAT_R32_UINT;
		case EVertexBufferLayoutElementTypes::INT: return VK_FORMAT_R32_SINT;
		case EVertexBufferLayoutElementTypes::ULONG: return VK_FORMAT_R64_UINT;
		case EVertexBufferLayoutElementTypes::LONG: return VK_FORMAT_R64_SINT;
		case EVertexBufferLayoutElementTypes::FLOAT: return VK_FORMAT_R32_SFLOAT;
		case EVertexBufferLayoutElementTypes::DOUBLE: return VK_FORMAT_R64_SFLOAT;
		case EVertexBufferLayoutElementTypes::VEC2: return VK_FORMAT_R32_SFLOAT;
		case EVertexBufferLayoutElementTypes::VEC3: return VK_FORMAT_R32_SFLOAT;
		case EVertexBufferLayoutElementTypes::VEC4: return VK_FORMAT_R32_SFLOAT;
		case EVertexBufferLayoutElementTypes::MAT2: return VK_FORMAT_R32_SFLOAT;
		case EVertexBufferLayoutElementTypes::MAT3: return VK_FORMAT_R32_SFLOAT;
		case EVertexBufferLayoutElementTypes::MAT4: return VK_FORMAT_R32_SFLOAT;
		default: return VkFormat(0);
	}
}

VulkanVertexBuffer::VulkanVertexBuffer(IGraphicsContext* aContext) : IVertexBuffer(aContext)
{
	mBuffer = nullptr;

	mBindingDescription = {};

	mSize = 0;
	mData = nullptr;
}

VulkanVertexBuffer::~VulkanVertexBuffer()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	vmaDestroyBuffer(context->getBufferAllocator(), mBuffer, mAllocation);
	mBuffer = nullptr;

	free(mData);
}

void VulkanVertexBuffer::construct(const VertexBufferCreateFlags& aInfo)
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);

	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.size = mSize;
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	const bool isExclusive = aInfo & EVertexBufferCreateFlagBits::VERTEX_BUFFER_SHARING_MODE_EXCLUSIVE;
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

void VulkanVertexBuffer::setData(void* aData, const size_t aSize)
{
	mSize = aSize;
	memcpy(mData, aData, aSize);
}

void VulkanVertexBuffer::setLayout(const VertexBufferLayout& aLayout)
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

