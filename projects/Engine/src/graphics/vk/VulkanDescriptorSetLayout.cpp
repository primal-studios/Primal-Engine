#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanSampler.h"
#include "core/PrimalCast.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(IGraphicsContext* aContext) : IDescriptorSetLayout(aContext)
{
	mLayout = nullptr;
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
	
}

void VulkanDescriptorSetLayout::construct(const DescriptorSetLayoutCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkDescriptorSetLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.flags = aInfo.flags;
	createInfo.bindingCount = static_cast<uint32_t>(aInfo.layoutBindings.size());

	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::vector<std::vector<VkSampler>> samplers;

	for (const auto& b : aInfo.layoutBindings)
	{
		std::vector<VkSampler> vkSamplers;
		for(const auto& s : b.immutableSamplers)
		{
			VulkanSampler* vkSampler = primal_cast<VulkanSampler*>(s);
			vkSamplers.push_back(vkSampler->getHandle());
		}
		samplers.push_back(vkSamplers);
	}

	uint32_t i = 0;
	for(const auto& b : aInfo.layoutBindings)
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = b.binding;
		binding.descriptorCount = static_cast<uint32_t>(b.immutableSamplers.size());
		binding.descriptorType = static_cast<VkDescriptorType>(b.type);

		binding.pImmutableSamplers = samplers[i].data();
		bindings.push_back(binding);
		i++;
	}
	createInfo.pBindings = bindings.data();
	createInfo.bindingCount = static_cast<uint32_t>(bindings.size());

	vkCreateDescriptorSetLayout(context->getDevice(), &createInfo, nullptr, &mLayout);
}