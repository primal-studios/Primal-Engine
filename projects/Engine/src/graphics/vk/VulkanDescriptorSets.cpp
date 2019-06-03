#include "graphics/vk/VulkanDescriptorSets.h"
#include "graphics/vk/VulkanDescriptorPool.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "core/Log.h"

VulkanDescriptorSets::VulkanDescriptorSets(IGraphicsContext* aContext)
	: IDescriptorSets(aContext)
{
	
}

VulkanDescriptorSets::~VulkanDescriptorSets()
{
	// Data is cleared on pool clear
}

void VulkanDescriptorSets::construct(const DescriptorSetCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	VulkanDescriptorPool* pool = primal_cast<VulkanDescriptorPool*>(aInfo.pool);
	allocInfo.descriptorPool = pool->getHandle();
	allocInfo.descriptorSetCount = aInfo.setLayouts.size();

	std::vector<VkDescriptorSetLayout> layouts;
	for(const auto& l : aInfo.setLayouts)
	{
		VulkanDescriptorSetLayout* layout = primal_cast<VulkanDescriptorSetLayout*>(l);
		VkDescriptorSetLayout set = layout->getHandle();

		layouts.push_back(set);
	}

	allocInfo.pSetLayouts = layouts.data();

	mSets.resize(aInfo.setLayouts.size());
	const VkResult result = vkAllocateDescriptorSets(context->getDevice(), &allocInfo, mSets.data());

	if (result != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create descriptor sets.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created descriptor sets.");
	}
}

VkDescriptorSet VulkanDescriptorSets::getSet(const size_t aIndex) const
{
	return mSets[aIndex];
}
