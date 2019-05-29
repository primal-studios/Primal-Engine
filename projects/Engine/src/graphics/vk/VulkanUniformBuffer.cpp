#include "graphics/vk/VulkanUniformBuffer.h"

VulkanUniformBuffer::VulkanUniformBuffer(IGraphicsContext* aContext) : IUniformBuffer(aContext)
{
	
}

VulkanUniformBuffer::~VulkanUniformBuffer()
{
	
}

void VulkanUniformBuffer::construct(const UniformBufferCreateInfo& aInfo)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0; // TODO: Where to get this from?
	uboLayoutBinding.descriptorType = static_cast<VkDescriptorType>(aInfo.descriptorType);
	uboLayoutBinding.descriptorCount = 1; // TODO: Do we want to support more?

	uboLayoutBinding.stageFlags = aInfo.shaderStageFlags;
	uboLayoutBinding.pImmutableSamplers = nullptr; // TODO: This is optional, do we want to support this?

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1; // TODO: Do we want to support more?
	layoutInfo.pBindings = &uboLayoutBinding;
}

void VulkanUniformBuffer::setData(void* aData, const size_t aSize)
{

}

void VulkanUniformBuffer::setLayout(const BufferLayout& aLayout)
{
	
}
