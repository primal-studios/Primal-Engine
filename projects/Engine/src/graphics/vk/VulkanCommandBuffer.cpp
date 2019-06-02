#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanFramebuffer.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanRenderPass.h"

#include <algorithm>

VulkanCommandBuffer::VulkanCommandBuffer(IGraphicsContext* aContext)
	: ICommandBuffer(aContext), mContext(aContext)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkSemaphoreCreateInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	{
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

	{
		const VkResult res = vkCreateFence(context->getDevice(), &fenceInfo, nullptr, &mFence);
		if (res != VK_SUCCESS)
		{
			PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan fence.");
		}
		else
		{
			PRIMAL_INTERNAL_INFO("Successfully created Vulkan fence.");
		}
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

void VulkanCommandBuffer::record(const CommandBufferRecordInfo& aInfo)
{
	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritanceInfo.framebuffer = primal_cast<VulkanFramebuffer*>(aInfo.inheritance->frameBuffer)->getHandle();
	inheritanceInfo.occlusionQueryEnable = aInfo.inheritance->occlusionQueryEnable;
	inheritanceInfo.pipelineStatistics = aInfo.inheritance->pipelineStatistics;
	inheritanceInfo.renderPass = primal_cast<VulkanRenderPass*>(aInfo.inheritance->renderPass)->getHandle();
	inheritanceInfo.subpass = aInfo.inheritance->subPass;
	inheritanceInfo.queryFlags = aInfo.inheritance->queryPrecsise ? VK_QUERY_CONTROL_PRECISE_BIT : 0;

	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = aInfo.flags;
	info.pInheritanceInfo = &inheritanceInfo;

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkBeginCommandBuffer(mBuffer, &info);
}

void VulkanCommandBuffer::end()
{
	vkEndCommandBuffer(mBuffer);
}

void VulkanCommandBuffer::recordRenderPass(const RenderPassRecordInfo& aInfo)
{
	VkRenderPassBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	info.framebuffer = primal_cast<VulkanFramebuffer*>(aInfo.frameBuffer)->getHandle();
	info.renderPass = primal_cast<VulkanRenderPass*>(aInfo.renderPass)->getHandle();
	info.renderArea = { {aInfo.renderArea.x, aInfo.renderArea.y}, {static_cast<uint32_t>(aInfo.renderArea.z), static_cast<uint32_t>(aInfo.renderArea.w)} };
	info.clearValueCount = static_cast<uint32_t>(aInfo.clearValues.size());

	std::vector<VkClearValue> clearValues;
	clearValues.reserve(info.clearValueCount);

	for (auto clear : aInfo.clearValues)
	{
		VkClearValue val;
		memcpy(val.color.float32, clear.color.float32, 4 * sizeof(float));
		clearValues.emplace_back(val);
	}

	info.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(mBuffer, &info, static_cast<VkSubpassContents>(aInfo.subpassContents));
}

void VulkanCommandBuffer::endRenderPass()
{
	vkCmdEndRenderPass(mBuffer);
}

void VulkanCommandBuffer::bindGraphicsPipeline(IGraphicsPipeline* aPipeline)
{
	VulkanGraphicsPipeline* pipeline = primal_cast<VulkanGraphicsPipeline*>(aPipeline);
	vkCmdBindPipeline(mBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
}

VkCommandBuffer VulkanCommandBuffer::getHandle() const
{
	return mBuffer;
}

VkSemaphore VulkanCommandBuffer::getSemaphore() const
{
	return mSemaphore;
}

std::vector<VkSemaphore>& VulkanCommandBuffer::getSemaphoresToSignal()
{
	return mSemDependsOnThis;
}

std::vector<VkSemaphore>& VulkanCommandBuffer::getSemaphoresToWaitOn()
{
	return mSemThisDependsOn;
}

VkFence& VulkanCommandBuffer::getFence()
{
	return mFence;
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
	vkDestroyFence(context->getDevice(), mFence, nullptr);
}
