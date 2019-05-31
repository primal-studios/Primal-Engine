#include "systems/RenderSystem.h"

#include "core/PrimalCast.h"
#include "ecs/EntityManager.h"

RenderSystem::RenderSystem(Window* aWindow)
	: mRenderPass(nullptr), mWindow(aWindow)
{
	GraphicsContextCreateInfo info;
	info.applicationName = "Sandbox";
	info.versionMajor = 0;
	info.versionMinor = 0;
	info.window = aWindow->getNativeWindow();

	mContext = new VulkanGraphicsContext(info);

	const CommandPoolCreateInfo commandPoolInfo = {
		COMMAND_POOL_RESET_COMMAND_BUFFER,
		mContext->getGraphicsQueueIndex()
	};

	mPool = new VulkanCommandPool(mContext);
	mPool->construct(commandPoolInfo);

	mSwapChain = new VulkanSwapChain(mContext);

	SwapChainCreateInfo swapChainInfo = {};
	swapChainInfo.surfaceHandle = reinterpret_cast<uint64_t>(mContext->getSurfaceHandle());
	swapChainInfo.width = aWindow->width();
	swapChainInfo.height = aWindow->height();
	swapChainInfo.maxImageCount = mFlightSize;

	mSwapChain->construct(swapChainInfo);
}

RenderSystem::~RenderSystem()
{
	vkDeviceWaitIdle(primal_cast<VulkanGraphicsContext*>(mContext)->getDevice());

	delete mSwapChain;

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		delete mFramebuffers[i];
		delete mPrimaryBuffer[i];
	}

	delete[] mFramebuffers;
	delete[] mPrimaryBuffer;

	delete mRenderPass;
	delete mPool;
	delete mContext;
}

void RenderSystem::initialize()
{
	mPrimaryBuffer = new VulkanCommandBuffer * [mFlightSize];

	const CommandBufferCreateInfo commandBufferInfo =
	{
		mPool,
		true
	};

	const AttachmentDescription colorAttachment{
		mSwapChain->getSwapchainFormat(),
		IMAGE_SAMPLE_1,
		EAttachmentLoadOperation::CLEAR,
		EAttachmentStoreOperation::STORE,
		EAttachmentLoadOperation::DONT_CARE,
		EAttachmentStoreOperation::DONT_CARE,
		IMAGE_LAYOUT_UNDEFINED,
		IMAGE_LAYOUT_PRESENT_SRC_KHR,
	};

	const AttachmentReference colorRef = {
		0,
		EImageLayout::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};


	SubPassDescription desc = {};
	desc.colors.push_back(colorRef);

	SubPassDependency dep = {};
	dep.dependencyFlags = 0;
	dep.srcSubPass = VK_SUBPASS_EXTERNAL;
	dep.dstSubPass = 0;
	dep.srcStages = PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
	dep.srcAccess = 0;
	dep.dstStages = PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT;
	dep.dstAccess = ACCESS_COLOR_ATTACHMENT_READ | ACCESS_COLOR_ATTACHMENT_WRITE;

	RenderPassCreateInfo renderPassInfo;
	renderPassInfo.attachments.push_back(colorAttachment);
	renderPassInfo.dependencies.push_back(dep);
	renderPassInfo.descriptions.push_back(desc);

	mRenderPass = new VulkanRenderPass(mContext);
	mRenderPass->construct(renderPassInfo);

	auto views = mSwapChain->getImageViews();

	uint32_t fbc = 0;

	mFramebuffers = new VulkanFramebuffer * [mFlightSize];
	for (const auto view : views)
	{
		FramebufferCreateInfo frameBufferInfo = {};
		frameBufferInfo.attachments.push_back(view);
		frameBufferInfo.renderPass = mRenderPass;
		frameBufferInfo.height = mWindow->height();
		frameBufferInfo.width = mWindow->width();
		frameBufferInfo.layers = 1;

		VulkanFramebuffer* fb = new VulkanFramebuffer(mContext);
		fb->construct(frameBufferInfo);
		*(mFramebuffers + fbc) = fb;

		fbc++;
	}

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		mPrimaryBuffer[i] = new VulkanCommandBuffer(mContext);
	}

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		mPrimaryBuffer[i]->construct(commandBufferInfo);
	}
}

void RenderSystem::preRender()
{
	
}

void RenderSystem::render()
{
	const auto handle = *(mPrimaryBuffer + mCurrentFrame);

	mPrimaryInheritance.renderPass = mRenderPass;
	mPrimaryInheritance.frameBuffer = mFramebuffers[mCurrentFrame];
	mPrimaryInheritance.occlusionQueryEnable = false;
	mPrimaryInheritance.queryPrecsise = false;
	mPrimaryInheritance.subPass = 0;
	mPrimaryInheritance.pipelineStatistics = 0;

	mPrimaryRecordInfo.inheritance = &mPrimaryInheritance;
	mPrimaryRecordInfo.flags = COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE;

	mSwapChain->beginFrame();

	handle->record(mPrimaryRecordInfo);

	// TODO: RENDER STUFF HERE

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = mRenderPass->getHandle();
	renderPassInfo.framebuffer = mFramebuffers[mCurrentFrame]->getHandle();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = {mWindow->width(), mWindow->height()};

	VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(handle->getHandle(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdEndRenderPass(handle->getHandle());

	handle->end();

	mSwapChain->submit(handle);
	mSwapChain->swap();

	mCurrentFrame = (mCurrentFrame + 1) % mFlightSize;
}

void RenderSystem::postRender()
{

}
