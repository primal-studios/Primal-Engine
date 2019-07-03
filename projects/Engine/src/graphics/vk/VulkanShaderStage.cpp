#include "graphics/vk/VulkanShaderStage.h"
#include "graphics/vk/VulkanShaderModule.h"
#include "core/PrimalCast.h"

VulkanShaderStage::VulkanShaderStage(IGraphicsContext* aContext) : IShaderStage(aContext)
{
	mStage = {};
	mData = nullptr;
}

void VulkanShaderStage::construct(const ShaderStageCreateInfo& aInfo)
{
	mStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	mStage.flags = aInfo.flags;
	mStage.stage = static_cast<VkShaderStageFlagBits>(aInfo.stage);

	VulkanShaderModule* module = primal_cast<VulkanShaderModule*>(aInfo.module);
	mStage.module = module->getModule();
	mData = malloc((aInfo.name.size() + 1) * sizeof(char));
	char* charData = static_cast<char*>(mData);

	for(size_t i = 0; i < aInfo.name.size(); i++)
	{
		charData[i] = aInfo.name[i];
	}
	charData[aInfo.name.size()] = '\0';

	mStage.pName = charData;
	mModule = aInfo.module;
}

VkPipelineShaderStageCreateInfo VulkanShaderStage::getStage() const
{
	return mStage;
}

IShaderModule* VulkanShaderStage::getModule() const
{
	return mModule;
}
