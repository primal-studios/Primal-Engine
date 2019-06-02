#include "systems/RenderSystem.h"

#include "core/PrimalCast.h"
#include "ecs/EntityManager.h"
#include "filesystem/FileSystem.h"
#include "graphics/vk/VulkanShaderModule.h"
#include "graphics/vk/VulkanShaderStage.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanPipelineLayout.h"

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

	delete mGraphicsPipeline;
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

	const auto vertSource = FileSystem::instance().getBytes("data/effects/vert.spv");
	const auto fragSource = FileSystem::instance().getBytes("data/effects/frag.spv");

	IShaderModule* vertModule = new VulkanShaderModule(mContext);
	vertModule->construct({ 0, vertSource });

	IShaderModule* fragModule = new VulkanShaderModule(mContext);
	fragModule->construct({ 0, fragSource });

	IShaderStage* vertStage = new VulkanShaderStage(mContext);
	vertStage->construct({ 0, EShaderStageFlagBits::SHADER_STAGE_VERTEX, vertModule, "main" });

	IShaderStage* fragStage = new VulkanShaderStage(mContext);
	fragStage->construct({ 0, EShaderStageFlagBits::SHADER_STAGE_FRAGMENT, fragModule, "main" });

	mGraphicsPipeline = new VulkanGraphicsPipeline(mContext);

	PipelineVertexStateCreateInfo vertexState = {};

	PipelineInputAssemblyStateCreateInfo assemblyState = {};
	assemblyState.topology = PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyState.primitiveRestartEnable = false;

	Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = mWindow->width();
	viewport.height = mWindow->height();
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	Vector4i rect = {};
	rect.x = 0.0f;
	rect.y = 0.0f;
	rect.z = mWindow->width();
	rect.w = mWindow->height();

	PipelineViewportStateCreateInfo viewportState = {};
	viewportState.flags = 0;
	viewportState.viewports = { viewport };
	viewportState.rectangles = { rect };

	PipelineRasterizationStateCreateInfo rasterizationState = {};
	rasterizationState.flags = 0;
	rasterizationState.depthClampEnable = false;
	rasterizationState.rasterizerDiscardEnable = false;
	rasterizationState.polygonMode = EPolygonMode::FILL;
	rasterizationState.lineWidth = 1.0f;
	rasterizationState.cullMode = ECullMode::BACK;
	rasterizationState.frontFace = EFrontFace::CLOCKWISE;
	rasterizationState.depthBiasEnable = false;

	PipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sampleShadingEnable = false;
	multisampleState.rasterizationSamples = SAMPLE_COUNT_1;

	PipelineColorBlendAttachmentState colorBlendStateAttachement = {};
	colorBlendStateAttachement.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendStateAttachement.blendEnable = false;

	PipelineColorBlendStateCreateInfo colorBlendState = {};
	colorBlendState.flags = 0;
	colorBlendState.attachments = { colorBlendStateAttachement };
	colorBlendState.logicOpEnable = false;
	colorBlendState.logicOp = ELogicOp::LOGIC_OP_COPY;
	colorBlendState.blendConstants[0] = 0.0f;
	colorBlendState.blendConstants[1] = 0.0f;
	colorBlendState.blendConstants[2] = 0.0f;
	colorBlendState.blendConstants[3] = 0.0f;

	IPipelineLayout* layout = new VulkanPipelineLayout(mContext);
	layout->construct({ 0, {}, {} });

	GraphicsPipelineCreateInfo createInfo = {};
	createInfo.flags = 0;
	createInfo.stages = { vertStage, fragStage };

	createInfo.vertexState = &vertexState;
	createInfo.assemblyState = &assemblyState;
	createInfo.viewportState = &viewportState;
	createInfo.rasterizationState = &rasterizationState;
	createInfo.multisampleState = &multisampleState;
	createInfo.colorBlendState = &colorBlendState;
	createInfo.layout = layout;
	createInfo.basePipelineHandle = nullptr;
	createInfo.basePipelineIndex = -1;
	createInfo.renderPass = mRenderPass;
	createInfo.subPass = 0;

	mGraphicsPipeline->construct(createInfo);

	delete vertStage;
	delete fragStage;

	delete vertModule;
	delete fragModule;

	delete layout;
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

	RenderPassRecordInfo recordInfo = {};
	recordInfo.renderPass = mRenderPass;
	recordInfo.frameBuffer = mFramebuffers[mCurrentFrame];
	recordInfo.renderArea = { 0, 0, static_cast<int32_t>(mWindow->width()), static_cast<int32_t>(mWindow->height()) };
	
	ClearValue clear = {};
	clear.color.float32[0] = 0.0f;
	clear.color.float32[1] = 0.0f;
	clear.color.float32[2] = 0.0f;
	clear.color.float32[3] = 0.0f;
	recordInfo.clearValues.push_back(clear);

	handle->recordRenderPass(recordInfo);

	handle->bindGraphicsPipeline(mGraphicsPipeline);

	// vkCmdDraw(handle->getHandle(), 3, 1, 0, 0);
	handle->draw(3, 1, 0, 0);

	handle->endRenderPass();

	handle->end();

	mSwapChain->submit(handle);
	mSwapChain->swap();

	mCurrentFrame = (mCurrentFrame + 1) % mFlightSize;
}

void RenderSystem::postRender()
{

}
