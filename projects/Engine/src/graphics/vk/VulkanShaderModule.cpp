#include "graphics/vk/VulkanShaderModule.h"
#include "graphics/vk/VulkanGraphicsContext.h"

#include "core/PrimalCast.h"
#include "core/Log.h"

VulkanShaderModule::VulkanShaderModule(IGraphicsContext* aContext) : IShaderModule(aContext)
{
	mModule = nullptr;
}

VulkanShaderModule::~VulkanShaderModule()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyShaderModule(context->getDevice(), mModule, nullptr);
}

void VulkanShaderModule::construct(const ShaderModuleCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = aInfo.code.size();
	createInfo.pCode = aInfo.code.data();
	createInfo.flags = aInfo.flags;

	const VkResult result = vkCreateShaderModule(context->getDevice(), &createInfo, nullptr, &mModule);

	if (result != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan shader module.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Succesfully created Vulkan shader module.");

	}
}

VkShaderModule VulkanShaderModule::getModule() const
{
	return mModule;
}
