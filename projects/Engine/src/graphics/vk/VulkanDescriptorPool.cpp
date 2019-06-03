#include "graphics/vk/VulkanDescriptorPool.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "core/PrimalCast.h"
#include "core/Log.h"

VulkanDescriptorPool::VulkanDescriptorPool(IGraphicsContext* aContext)
	: IDescriptorPool(aContext), mPool(nullptr)
{
	
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	vkDestroyDescriptorPool(context->getDevice(), mPool, nullptr);
}

void VulkanDescriptorPool::construct(const DescriptorPoolCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	std::vector<VkDescriptorPoolSize> vulkanPoolSizes;
	for(const auto& size : aInfo.poolSizes)
	{
		VkDescriptorPoolSize s = {};
		s.type = static_cast<VkDescriptorType>(size.type);
		s.descriptorCount = size.count;

		vulkanPoolSizes.push_back(s);
	}

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = aInfo.flags;
	poolInfo.poolSizeCount = vulkanPoolSizes.size();
	poolInfo.pPoolSizes = vulkanPoolSizes.data();
	poolInfo.maxSets = aInfo.maxSets;

	const VkResult result = vkCreateDescriptorPool(context->getDevice(), &poolInfo, nullptr, &mPool);
	if (result != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan descriptor pool.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan descriptor pool.");
	}
}

VkDescriptorPool VulkanDescriptorPool::getHandle() const
{
	return mPool;
}
