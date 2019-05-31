#include "core/Log.h"
#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanGraphicsContext.h"


VulkanCommandPool::VulkanCommandPool(IGraphicsContext* aContext)
	: ICommandPool(aContext)
{
}

VulkanCommandPool::~VulkanCommandPool()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyCommandPool(context->getDevice(), mPool, nullptr);
}

void VulkanCommandPool::construct(const CommandPoolCreateInfo& aInfo)
{
	VkCommandPoolCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = aInfo.flags;
	info.queueFamilyIndex = aInfo.queueFamilyIndex;

	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);

	const VkResult res = vkCreateCommandPool(context->getDevice(), &info, nullptr, &mPool);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan command pool.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan command pool.");
	}
}

VkCommandPool VulkanCommandPool::getPool() const
{
	return mPool;
}
