#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"

VulkanPipelineLayout::VulkanPipelineLayout(IGraphicsContext* aContext) 
	: IPipelineLayout(aContext)
{
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
	_destroy();
}

void VulkanPipelineLayout::construct(const PipelineLayoutCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkPipelineLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.flags = aInfo.flags;

	std::vector<VkDescriptorSetLayout> layouts;
	for(const auto& l : aInfo.setLayouts)
	{
		VulkanDescriptorSetLayout* vkLayout = primal_cast<VulkanDescriptorSetLayout*>(l);
		layouts.push_back(vkLayout->getHandle());
	}

	createInfo.pSetLayouts = layouts.data();
	createInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());

	std::vector<VkPushConstantRange> ranges;
	for(const auto& r: aInfo.pushConstantRanges)
	{
		VkPushConstantRange range = {};
		range.stageFlags = r.stageFlags;
		range.offset = r.offset;
		range.size = r.size;

		ranges.push_back(range);
	}

	createInfo.pPushConstantRanges = ranges.data();
	createInfo.pushConstantRangeCount = static_cast<uint32_t>(ranges.size());

	vkCreatePipelineLayout(context->getDevice(), &createInfo, nullptr, &mLayout);
}

void VulkanPipelineLayout::reconstruct(const PipelineLayoutCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

VkPipelineLayout VulkanPipelineLayout::getHandle() const
{
	return mLayout;
}

void VulkanPipelineLayout::_destroy() const
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyPipelineLayout(context->getDevice(), mLayout, nullptr);
}
