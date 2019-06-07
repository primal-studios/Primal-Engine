#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanSampler.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(IGraphicsContext* aContext) 
	: IDescriptorSetLayout(aContext)
{
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	_destroy();
}

void VulkanDescriptorSetLayout::construct(const DescriptorSetLayoutCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::vector<std::vector<VkSampler>> samplers;

	for(const auto& binding : aInfo.layoutBindings)
	{
		VkDescriptorSetLayoutBinding vkBinding = {};
		vkBinding.binding = binding.binding;
		vkBinding.descriptorType = static_cast<VkDescriptorType>(binding.descriptorType);
		vkBinding.descriptorCount = binding.descriptorCount;
		vkBinding.stageFlags = binding.shaderStageFlags;

		std::vector<VkSampler> bindingSamplers;
		for(const auto& sampler : binding.immutableSamplers)
		{
			VulkanSampler* vSampler = primal_cast<VulkanSampler*>(sampler);
			bindingSamplers.push_back(vSampler->getHandle());
		}
		samplers.push_back(bindingSamplers);

		bindings.push_back(vkBinding);
	}

	for(size_t i = 0; i < samplers.size(); i++)
	{
		if(samplers[i].empty())
		{
			bindings[i].pImmutableSamplers = nullptr;
			continue;
		}

		bindings[i].pImmutableSamplers = samplers[i].data();
	}

	VkDescriptorSetLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.flags = aInfo.flags;
	createInfo.bindingCount = static_cast<uint32_t>(aInfo.layoutBindings.size());
	createInfo.pBindings = bindings.data();
	
	const VkResult res = vkCreateDescriptorSetLayout(context->getDevice(), &createInfo, nullptr, &mLayout);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan descriptor set layout.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan descriptor set layout.");
	}
}

void VulkanDescriptorSetLayout::reconstruct(const DescriptorSetLayoutCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::getHandle() const
{
	return mLayout;
}

void VulkanDescriptorSetLayout::_destroy() const
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyDescriptorSetLayout(context->getDevice(), mLayout, nullptr);
}
