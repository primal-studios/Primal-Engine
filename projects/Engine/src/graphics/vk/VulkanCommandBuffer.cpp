#include <core/Log.h>
#include <core/PrimalCast.h>
#include <graphics/vk/VulkanCommandBuffer.h>
#include <graphics/vk/VulkanCommandPool.h>
#include <graphics/vk/VulkanGraphicsContext.h>

#include <algorithm>

VulkanCommandBuffer::VulkanCommandBuffer(IGraphicsContext* aContext)
	: ICommandBuffer(aContext), mContext(aContext)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkSemaphoreCreateInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	const VkResult res = vkCreateSemaphore(context->getDevice(), &semInfo, nullptr, &mSemaphore);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan semaphore.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan semaphore.");
	}
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	_destroy();
}

void VulkanCommandBuffer::addDependency(ICommandBuffer* aDependsOn)
{
	mThisDependsOn.push_back(primal_cast<VulkanCommandBuffer*>(aDependsOn));
	primal_cast<VulkanCommandBuffer*>(aDependsOn)->mDependsOnThis.push_back(this);
}

void VulkanCommandBuffer::removeDependency(ICommandBuffer* aDependsOn)
{
	const auto it = std::find(mThisDependsOn.begin(), mThisDependsOn.end(), primal_cast<VulkanCommandBuffer*>(aDependsOn));
	mThisDependsOn.erase(it);

	const auto otherIt = std::find(primal_cast<VulkanCommandBuffer*>(aDependsOn)->mDependsOnThis.begin(), primal_cast<VulkanCommandBuffer*>(aDependsOn)->mDependsOnThis.end(), this);
	primal_cast<VulkanCommandBuffer*>(aDependsOn)->mDependsOnThis.erase(otherIt);
}

void VulkanCommandBuffer::construct(const CommandBufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	VulkanCommandPool* pool = primal_cast<VulkanCommandPool*>(aInfo.pool);
	const VkCommandBufferLevel level = aInfo.primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;

	VkCommandBufferAllocateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.commandBufferCount = 1;
	info.commandPool = pool->getPool();
	info.level = level;

	const VkResult res = vkAllocateCommandBuffers(context->getDevice(), &info, &mBuffer);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan command buffer.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan command buffer.");
	}

	for (auto dependsOnThis : mDependsOnThis)
	{
		mSemDependsOnThis.push_back(dependsOnThis->getSemaphore());
	}

	for (auto thisDependsOn : mThisDependsOn)
	{
		mSemThisDependsOn.push_back(thisDependsOn->getSemaphore());
	}

	mPool = pool->getPool();
}

void VulkanCommandBuffer::reconstruct(const CommandBufferCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

VkCommandBuffer VulkanCommandBuffer::getHandle() const
{
	return mBuffer;
}

VkSemaphore VulkanCommandBuffer::getSemaphore() const
{
	return mSemaphore;
}

void VulkanCommandBuffer::_destroy() 
{
	mDependsOnThis.clear();
	mThisDependsOn.clear();
	mSemDependsOnThis.clear();
	mSemThisDependsOn.clear();

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkFreeCommandBuffers(context->getDevice(), mPool, 1, &mBuffer);
	vkDestroySemaphore(context->getDevice(), mSemaphore, nullptr);
}
