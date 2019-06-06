#include "graphics/vk/VulkanDescriptorSet.h"
#include "graphics/vk/VulkanDescriptorPool.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "core/Log.h"

VulkanDescriptorSet::VulkanDescriptorSet(IGraphicsContext* aContext)
	: IDescriptorSet(aContext), mSet(VK_NULL_HANDLE)
{
	
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{

}

void VulkanDescriptorSet::construct(const DescriptorSetCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	VulkanDescriptorPool* pool = primal_cast<VulkanDescriptorPool*>(aInfo.pool);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool->getHandle();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(aInfo.setLayouts.size());

	std::vector<VkDescriptorSetLayout> layouts;
	for(const auto& l : aInfo.setLayouts)
	{
		VulkanDescriptorSetLayout* layout = primal_cast<VulkanDescriptorSetLayout*>(l);
		VkDescriptorSetLayout set = layout->getHandle();

		layouts.push_back(set);
	}

	allocInfo.pSetLayouts = layouts.data();

	const VkResult result = vkAllocateDescriptorSets(context->getDevice(), &allocInfo, &mSet);

	if (result != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create descriptor sets.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created descriptor sets.");
	}
}

VkDescriptorSet VulkanDescriptorSet::getHandle() const
{
	return mSet;
}
