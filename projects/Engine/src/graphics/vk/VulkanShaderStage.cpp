#include "graphics/vk/VulkanShaderStage.h"
#include "graphics/vk/VulkanShaderModule.h"
#include "core/PrimalCast.h"

VulkanShaderStage::VulkanShaderStage(IGraphicsContext* aContext) : IShaderStage(aContext)
{
	mStage = {};
}

void VulkanShaderStage::construct(const ShaderStageCreateInfo& aInfo)
{
	mStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mStage.flags = aInfo.flags;
	mStage.stage = static_cast<VkShaderStageFlagBits>(aInfo.stage);

	VulkanShaderModule* module = primal_cast<VulkanShaderModule*>(aInfo.module);
	mStage.module = module->getModule();
	mStage.pName = aInfo.name.c_str();
}

VkPipelineShaderStageCreateInfo VulkanShaderStage::getStage() const
{
	return mStage;
}
