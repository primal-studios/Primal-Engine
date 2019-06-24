#include "systems/VulkanRenderSystem.h"
#include "core/PrimalCast.h"

VulkanRenderSystem::VulkanRenderSystem(Window* aWindow)
{
	mWindow = aWindow;

	GraphicsContextCreateInfo info;
	info.applicationName = "Sandbox";
	info.versionMajor = 0;
	info.versionMinor = 0;
	info.window = aWindow->getNativeWindow();

	mContext = new VulkanGraphicsContext(info);
	mSwapChain = new VulkanSwapChain(mContext);

	SwapChainCreateInfo swapChainInfo = {};
	swapChainInfo.surfaceHandle = reinterpret_cast<uint64_t>(mContext->getSurfaceHandle());
	swapChainInfo.width = aWindow->width();
	swapChainInfo.height = aWindow->height();
	swapChainInfo.maxImageCount = mFlightSize;

	mSwapChain->construct(swapChainInfo);
}

VulkanRenderSystem::~VulkanRenderSystem()
{
	mContext->idle();

	for(size_t i = 0; i < mCommandBuffers.size(); i++)
	{
		delete mCommandBuffers[i];
	}

	mCommandBuffers.clear();

	for(size_t i = 0; i < mFrameBuffers.size(); i++)
	{
		delete mFrameBuffers[i];
	}

	mFrameBuffers.clear();

	delete mRenderPass;

	delete mSwapChain;
	delete mContext;
}

void VulkanRenderSystem::initialize()
{
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

	const AttachmentDescription depthAttachment{
		static_cast<EDataFormat>(mSwapChain->getDepthFormat()),
		IMAGE_SAMPLE_1,
		EAttachmentLoadOperation::CLEAR,
		EAttachmentStoreOperation::DONT_CARE,
		EAttachmentLoadOperation::DONT_CARE,
		EAttachmentStoreOperation::DONT_CARE,
		IMAGE_LAYOUT_UNDEFINED,
		IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	const AttachmentReference colorRef = {
		0,
		EImageLayout::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	const AttachmentReference depthRef = {
		1,
		EImageLayout::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	SubPassDescription desc = {};
	desc.colors.push_back(colorRef);
	desc.depthStencil = depthRef;

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
	renderPassInfo.attachments.push_back(depthAttachment);
	renderPassInfo.dependencies.push_back(dep);
	renderPassInfo.descriptions.push_back(desc);

	mRenderPass = new VulkanRenderPass(mContext);
	mRenderPass->construct(renderPassInfo);

	auto views = mSwapChain->getImageViews();
	for (const auto view : views)
	{
		FramebufferCreateInfo frameBufferInfo = {};
		frameBufferInfo.attachments.push_back(view);
		frameBufferInfo.attachments.push_back(mSwapChain->getDepthView());
		frameBufferInfo.renderPass = mRenderPass;
		frameBufferInfo.height = mWindow->height();
		frameBufferInfo.width = mWindow->width();
		frameBufferInfo.layers = 1;

		VulkanFramebuffer* fb = new VulkanFramebuffer(mContext);
		fb->construct(frameBufferInfo);

		mFrameBuffers.push_back(fb);
	}

	for(uint32_t i = 0; i < mFlightSize; i++)
	{
		VulkanCommandBuffer* primaryBuffer = new VulkanCommandBuffer(mContext);
		const CommandBufferCreateInfo commandBufferInfo =
		{
			mContext->getCommandPool(),
			true
		};
		primaryBuffer->construct(commandBufferInfo);

		mCommandBuffers.push_back(primaryBuffer);
	}
}

void VulkanRenderSystem::preRender()
{
	
}

void VulkanRenderSystem::render()
{
	const auto handle = mCommandBuffers[mCurrentFrame];

	CommandBufferInheritanceInfo primaryInheritance = {};
	CommandBufferRecordInfo primaryRecordInfo = {};

	primaryInheritance.renderPass = mRenderPass;
	primaryInheritance.frameBuffer = mFrameBuffers[mCurrentFrame];
	primaryInheritance.occlusionQueryEnable = false;
	primaryInheritance.queryPrecsise = false;
	primaryInheritance.subPass = 0;
	primaryInheritance.pipelineStatistics = 0;

	primaryRecordInfo.inheritance = &primaryInheritance;
	primaryRecordInfo.flags = COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE;

	mSwapChain->beginFrame();

	handle->record(primaryRecordInfo);

	RenderPassRecordInfo recordInfo = {};
	recordInfo.renderPass = mRenderPass;
	recordInfo.frameBuffer = mFrameBuffers[mCurrentFrame];
	recordInfo.renderArea = { 0, 0, static_cast<int32_t>(mWindow->width()), static_cast<int32_t>(mWindow->height()) };

	ClearValue clear = {};
	clear.color.float32[0] = 0.0f;
	clear.color.float32[1] = 0.0f;
	clear.color.float32[2] = 0.0f;
	clear.color.float32[3] = 0.0f;

	ClearValue depth;
	depth.depthStencil = { 1.0f, 0 };
	recordInfo.clearValues.push_back(clear);
	recordInfo.clearValues.push_back(depth);

	handle->recordRenderPass(recordInfo);

	handle->endRenderPass();
	handle->end();

	mSwapChain->submit(handle);
	mSwapChain->swap();

	mCurrentFrame = (mCurrentFrame + 1) % mFlightSize;
}

void VulkanRenderSystem::postRender()
{
	
}

void VulkanRenderSystem::onEvent(Event& aEvent)
{
	
}