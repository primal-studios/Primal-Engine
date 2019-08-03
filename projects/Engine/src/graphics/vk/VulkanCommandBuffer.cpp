#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanDescriptorSet.h"
#include "graphics/vk/VulkanFramebuffer.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanRenderPass.h"
#include "graphics/vk/VulkanSwapChain.h"
#include "graphics/vk/VulkanTexture.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanVertexBuffer.h"

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
	
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	vkDestroySemaphore(context->getDevice(), mSemaphore, nullptr);
	vkDestroyFence(context->getDevice(), mFence, nullptr);
}

void VulkanCommandBuffer::addDependency(ICommandBuffer* aDependsOn)
{
	VulkanCommandBuffer* other = primal_cast<VulkanCommandBuffer*>(aDependsOn);

	VkSemaphore thisSem = getSemaphore();

	other->mSemsToSignal.push_back(thisSem);
	mSemsToWaitOn.push_back(thisSem);

	mThisDependsOn.push_back(primal_cast<VulkanCommandBuffer*>(aDependsOn));
	other->mDependsOnThis.push_back(this);
}

void VulkanCommandBuffer::removeDependency(ICommandBuffer* aDependsOn)
{
	VulkanCommandBuffer* other = primal_cast<VulkanCommandBuffer*>(aDependsOn);

	mThisDependsOn.erase(std::find(mThisDependsOn.begin(), mThisDependsOn.end(), aDependsOn));
	other->mDependsOnThis.erase(std::find(other->mDependsOnThis.begin(), other->mDependsOnThis.end(), this));

	other->mSemsToSignal.erase(std::find(other->mSemsToSignal.begin(), other->mSemsToSignal.end(), getSemaphore()));
	mSemsToWaitOn.erase(std::find(mSemsToWaitOn.begin(), mSemsToWaitOn.end(), getSemaphore()));
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
		mSemsToWaitOn.push_back(dependsOnThis->getSemaphore());
	}

	for (auto thisDependsOn : mThisDependsOn)
	{
		mSemsToSignal.push_back(thisDependsOn->getSemaphore());
	}

	mPool = pool->getPool();
}

void VulkanCommandBuffer::reconstruct(const CommandBufferCreateInfo& aInfo)
{
	destroy();
	construct(aInfo);
}

void VulkanCommandBuffer::destroy()
{
	_destroy();
}

void VulkanCommandBuffer::record(const CommandBufferRecordInfo& aInfo)
{
	for (BufferAllocation allocation : mBuffersToFree)
	{
		vmaDestroyBuffer(allocation.allocator, allocation.buffer, allocation.allocation);
	}
	mBuffersToFree.clear();

	VkCommandBufferInheritanceInfo inheritanceInfo = {};
	if (aInfo.inheritance != nullptr) {
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.framebuffer = primal_cast<VulkanFramebuffer*>(aInfo.inheritance->frameBuffer)->getHandle();
		inheritanceInfo.occlusionQueryEnable = aInfo.inheritance->occlusionQueryEnable;
		inheritanceInfo.pipelineStatistics = aInfo.inheritance->pipelineStatistics;
		inheritanceInfo.renderPass = primal_cast<VulkanRenderPass*>(aInfo.inheritance->renderPass)->getHandle();
		inheritanceInfo.subpass = aInfo.inheritance->subPass;
		inheritanceInfo.queryFlags = aInfo.inheritance->queryPrecsise ? VK_QUERY_CONTROL_PRECISE_BIT : 0;
	}

	VkCommandBufferBeginInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.flags = aInfo.flags;
	if (aInfo.inheritance != nullptr) {
		info.pInheritanceInfo = &inheritanceInfo;
	}

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

void VulkanCommandBuffer::copyBuffers(ISwapChain* aSwapchain, IVertexBuffer* aBuffer, void* aData, const size_t aSize)
{
	VulkanVertexBuffer* vb = primal_cast<VulkanVertexBuffer*>(aBuffer);
	VulkanSwapChain* sc = primal_cast<VulkanSwapChain*>(aSwapchain);
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	const bool usesStaging = (vb->mInfo.usage & BUFFER_USAGE_TRANSFER_DST) != 0;
	const bool isExclusive = (vb->mInfo.sharingMode == SHARING_MODE_EXCLUSIVE);

	if (usesStaging) 
	{
		if (vb->mStagingBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(context->getBufferAllocator(), vb->mStagingBuffer, vb->mStagingAllocation);
			vb->mStagingBuffer = VK_NULL_HANDLE;
		}

		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = aSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo stagingAllocInfo = {};
		stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &stagingBufferInfo, &stagingAllocInfo, &vb->mStagingBuffer, &vb->mStagingAllocation, nullptr);
	
		void* data;
		vmaMapMemory(context->getBufferAllocator(), vb->mStagingAllocation, &data);
		memcpy(data, aData, aSize);
		vmaUnmapMemory(context->getBufferAllocator(), vb->mStagingAllocation);

		VkBufferCopy copyRegion = {};
		copyRegion.size = aSize;
		vkCmdCopyBuffer(mBuffer, vb->mStagingBuffer, vb->mBuffer, 1, &copyRegion);

		mBuffersToFree.push_back(VulkanCommandBuffer::BufferAllocation{ context->getBufferAllocator(), vb->mStagingAllocation, vb->mStagingBuffer });
	}
	else
	{
		void* data;
		vmaMapMemory(context->getBufferAllocator(), vb->mAllocation, &data);
		memcpy(data, mData, aSize);  // NOLINT(bugprone-undefined-memory-manipulation)
		vmaUnmapMemory(context->getBufferAllocator(), vb->mAllocation);
	}
}

void VulkanCommandBuffer::copyBuffers(ISwapChain* aSwapchain, IIndexBuffer* aBuffer, void* aData, const size_t aSize)
{
	VulkanIndexBuffer* vb = primal_cast<VulkanIndexBuffer*>(aBuffer);
	VulkanSwapChain* sc = primal_cast<VulkanSwapChain*>(aSwapchain);
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	const bool usesStaging = (vb->mInfo.usage & BUFFER_USAGE_TRANSFER_DST) != 0;
	const bool isExclusive = (vb->mInfo.sharingMode == SHARING_MODE_EXCLUSIVE);

	if (usesStaging)
	{
		if (vb->mStagingBuffer != VK_NULL_HANDLE)
		{
			vmaDestroyBuffer(context->getBufferAllocator(), vb->mStagingBuffer, vb->mStagingAllocation);
			vb->mStagingBuffer = VK_NULL_HANDLE;
		}

		VkBufferCreateInfo stagingBufferInfo = {};
		stagingBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferInfo.size = aSize;
		stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VmaAllocationCreateInfo stagingAllocInfo = {};
		stagingAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		vmaCreateBuffer(context->getBufferAllocator(), &stagingBufferInfo, &stagingAllocInfo, &vb->mStagingBuffer, &vb->mStagingAllocation, nullptr);

		void* data;
		vmaMapMemory(context->getBufferAllocator(), vb->mStagingAllocation, &data);
		memcpy(data, aData, aSize);
		vmaUnmapMemory(context->getBufferAllocator(), vb->mStagingAllocation);

		VkBufferCopy copyRegion = {};
		copyRegion.size = aSize;
		vkCmdCopyBuffer(mBuffer, vb->mStagingBuffer, vb->mBuffer, 1, &copyRegion);

		mBuffersToFree.push_back(VulkanCommandBuffer::BufferAllocation{ context->getBufferAllocator(), vb->mStagingAllocation, vb->mStagingBuffer });
	}
	else
	{
		void* data;
		vmaMapMemory(context->getBufferAllocator(), vb->mAllocation, &data);
		memcpy(data, mData, aSize);  // NOLINT(bugprone-undefined-memory-manipulation)
		vmaUnmapMemory(context->getBufferAllocator(), vb->mAllocation);
	}
}

void VulkanCommandBuffer::bindGraphicsPipeline(IGraphicsPipeline* aPipeline)
{
	VulkanGraphicsPipeline* pipeline = primal_cast<VulkanGraphicsPipeline*>(aPipeline);
	vkCmdBindPipeline(mBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
}

void VulkanCommandBuffer::bindVertexBuffers(uint32_t aFirstBinding, uint32_t aBindingCount,
	std::vector<IVertexBuffer*> aBuffers, std::vector<uint64_t> aOffsets)
{
	std::vector<VkBuffer> buffers;
	for(const auto& b : aBuffers)
	{
		VulkanVertexBuffer* vkBuffer = primal_cast<VulkanVertexBuffer*>(b);
		buffers.push_back(vkBuffer->mBuffer);
	}
	vkCmdBindVertexBuffers(mBuffer, aFirstBinding, aBindingCount, buffers.data(), aOffsets.data());
}

void VulkanCommandBuffer::bindIndexBuffer(IIndexBuffer* aBuffer, uint64_t aOffset, EIndexType aType)
{
	VulkanIndexBuffer* iBuffer = primal_cast<VulkanIndexBuffer*>(aBuffer);
	vkCmdBindIndexBuffer(mBuffer, iBuffer->mBuffer, aOffset, static_cast<VkIndexType>(aType));
}

void VulkanCommandBuffer::bindMaterial(Material* aMaterial, const uint32_t aFrame)
{
	const VkDescriptorSet set = primal_cast<VulkanDescriptorSet*>(aMaterial->mSet.set)->getHandle(aFrame);
	VulkanDescriptorSet* sceneSet = nullptr;
	std::vector<VkWriteDescriptorSet> writeSets;

	bool setScene = false;
	if (mData != nullptr) {
		sceneSet = primal_cast<VulkanDescriptorSet*>(mData->mSet);
		VulkanUniformBuffer* vubo = primal_cast<VulkanUniformBuffer*>(mData->mUboPool->getBuffer(0));
		OffsetSize sz = { 0, mData->mUboPool->getStrideSize() };
		const WriteDescriptorSet writeDesc = vubo->getWriteDescriptor(mData->mUboPool->getBindingPoint(), sz, false);
		auto writeDescSet = writeDesc.getWriteDescriptorSet();
		writeDescSet.dstSet = sceneSet->getHandle(aFrame);
		writeSets.push_back(writeDescSet);
		vubo->setData(mData->mCpuBacking, 0, mData->mBackingSz);
		mData = nullptr;
		setScene = true;
	}

	if (aMaterial->mDirtyBit)
	{
		Material* parent = aMaterial->_getRootAncestor();

		for (UniformBufferPool* uboPool : parent->mUboLayouts)
		{
			for (IUniformBuffer* ubo : uboPool->getBuffers())
			{
				VulkanUniformBuffer* vubo = primal_cast<VulkanUniformBuffer*>(ubo);
				OffsetSize sz = { 0, uboPool->getStrideSize() };
				WriteDescriptorSet writeDesc = vubo->getWriteDescriptor(uboPool->getBindingPoint(), sz);
				auto writeDescSet = writeDesc.getWriteDescriptorSet();
				writeDescSet.dstSet = set;
				writeSets.push_back(writeDescSet);
			}
		}

		for (const auto& tex : aMaterial->_getActiveTextures())
		{
			VulkanTexture* vtex = primal_cast<VulkanTexture*>(tex);
			auto writeDesc = vtex->getWriteDescriptor(tex->getBindingPoint(), {});
			auto writeDescSet = writeDesc.getWriteDescriptorSet();
			writeDescSet.dstSet = set;
			writeSets.push_back(writeDescSet);
		}

		--aMaterial->mDirtyBit;
	}

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkUpdateDescriptorSets(context->getDevice(), static_cast<uint32_t>(writeSets.size()), writeSets.data(), 0, nullptr);

	for (const auto& ubo : aMaterial->mBackingBuffers)
	{
		auto buffers = ubo.first->getBuffers();
		for (size_t i = 0; i < buffers.size(); i++)
		{
			void* data = ubo.second.blocks[i];
			buffers[i]->setData(data, 0, 65536);
		}
	}

	for (const auto& writeSet : writeSets)
	{
		delete writeSet.pBufferInfo;
		delete writeSet.pImageInfo;
	}

	VkDescriptorSet sets[2];
	uint32_t offset = 0;
	uint32_t count = 0;

	if (setScene)
	{
		sets[0] = sceneSet->getHandle(aFrame);
		sets[1] = set;
		count = 2;
		offset = 0;
	}
	else
	{
		sets[0] = set;
		count = 1;
		offset = 1;
	}

	VulkanPipelineLayout* layout = primal_cast<VulkanGraphicsPipeline*>(aMaterial->mPipeline)->getLayout();
	uint32_t dynamicOffset = 0; // TODO : Calculate the number of dynamic buffers associated
	vkCmdBindDescriptorSets(mBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout->getHandle(), offset, count, sets, 1, &dynamicOffset);
}

void VulkanCommandBuffer::bindMaterialInstance(MaterialInstance* aInstance, uint32_t aFrame)
{
	Material* parent = aInstance->mParent->_getRootAncestor();

	VkDescriptorSet set = primal_cast<VulkanDescriptorSet*>(aInstance->mParent->mSet.set)->getHandle(aFrame);

	VulkanGraphicsPipeline* pipeline = primal_cast<VulkanGraphicsPipeline*>(parent->mPipeline);
	VulkanPipelineLayout* layout = pipeline->getLayout();

	std::vector<uint32_t> offsets;
	for (const auto& pair : parent->mBackingBuffers)
	{
		const auto size = pair.second.elementSize;
		const auto count = pair.second.elementCount;
		const auto index = aInstance->mInstanceId / count;
		const auto offset = (aInstance->mInstanceId - index * count) * size;
		offsets.push_back(static_cast<uint32_t>(offset));
	}

	vkCmdBindDescriptorSets(mBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout->getHandle(), 1, 1, &set, static_cast<uint32_t>(offsets.size()), offsets.data());
}

void VulkanCommandBuffer::bindSceneData(SceneData* aData, const uint32_t aFrame)
{
	mData = aData;
}

void VulkanCommandBuffer::draw(const uint32_t aVertexCount, const uint32_t aInstanceCount, const uint32_t aFirstVertex,
                               const uint32_t aFirstInstance)
{
	vkCmdDraw(mBuffer, aVertexCount, aInstanceCount, aFirstVertex, aFirstInstance);
}

void VulkanCommandBuffer::drawIndexed(const uint32_t aIndexCount, const uint32_t aInstanceCount, const uint32_t aFirstIndex,
	const int32_t aVertexOffset, const uint32_t aFirstInstance)
{
	vkCmdDrawIndexed(mBuffer, aIndexCount, aInstanceCount, aFirstIndex, aVertexOffset, aFirstInstance);
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
	return mSemsToSignal;
}

std::vector<VkSemaphore>& VulkanCommandBuffer::getSemaphoresToWaitOn()
{
	return mSemsToWaitOn;
}

VkFence& VulkanCommandBuffer::getFence()
{
	return mFence;
}

void VulkanCommandBuffer::_destroy() 
{
	for (const auto& allocation : mBuffersToFree)
	{
		vmaDestroyBuffer(allocation.allocator, allocation.buffer, allocation.allocation);
	}
	mBuffersToFree.clear();

	mDependsOnThis.clear();
	mThisDependsOn.clear();
	mSemsToWaitOn.clear();
	mSemsToSignal.clear();

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkFreeCommandBuffers(context->getDevice(), mPool, 1, &mBuffer);
}
