#include "systems/RenderSystem.h"

#include "core/PrimalCast.h"
#include "ecs/EntityManager.h"
#include "filesystem/FileSystem.h"
#include "graphics/vk/VulkanShaderModule.h"
#include "graphics/vk/VulkanShaderStage.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanVertexBuffer.h"
#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanDescriptorPool.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanDescriptorSet.h"

#include <stb/stb_image.h>

#include "assets/AssetManager.h"
#include "graphics/vk/VulkanSampler.h"
#include "assets/MeshAsset.h"

RenderSystem::RenderSystem(Window* aWindow)
	: mRenderPass(nullptr), mGraphicsPipeline(nullptr), mLayout(nullptr), mVertexBuffer(nullptr), mIndexBuffer(nullptr),
	  mUniformBuffer(nullptr),
	  mDescriptorPool(nullptr), mSet(nullptr), mSetLayout(nullptr), mTexture(nullptr),
	  mWindow(aWindow)
{
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

	std::vector<DescriptorPoolSize> poolSizes;
	DescriptorPoolSize uniformBufferSize;
	uniformBufferSize.type = EDescriptorType::UNIFORM_BUFFER;
	uniformBufferSize.count = 2;

	DescriptorPoolSize combinedSamplerSize;
	combinedSamplerSize.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
	combinedSamplerSize.count = 4;

	poolSizes.push_back(combinedSamplerSize);

	DescriptorPoolCreateInfo createInfo;
	createInfo.flags = 0;
	createInfo.maxSets = 6;
	createInfo.poolSizes = poolSizes;

	mDescriptorPool = new VulkanDescriptorPool(mContext);
	mDescriptorPool->construct(createInfo);
}

RenderSystem::~RenderSystem()
{
	vkDeviceWaitIdle(primal_cast<VulkanGraphicsContext*>(mContext)->getDevice());

	AssetManager::instance().unloadAll();

	delete mTexture;
	delete mLayout;
	delete mSetLayout;

	delete mSwapChain;

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		delete mFramebuffers[i];
		delete mPrimaryBuffer[i];
	}

	delete[] mFramebuffers;
	delete[] mPrimaryBuffer;

	delete mUniformBuffer;

	delete mDescriptorPool;

	delete mGraphicsPipeline;
	delete mRenderPass;
	delete mContext;
}

struct UBO
{
	Matrix4f model;
	Matrix4f view;
	Matrix4f proj;
	Matrix4f mvp;
};

void RenderSystem::initialize()
{
	mPrimaryBuffer = new VulkanCommandBuffer * [mFlightSize];

	const CommandBufferCreateInfo commandBufferInfo =
	{
		mContext->getCommandPool(),
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

	uint32_t fbc = 0;

	mFramebuffers = new VulkanFramebuffer * [mFlightSize];
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

	auto meshAsset = AssetManager::instance().load<MeshAsset>("mesh", "data/models/cube.glb");
	auto mesh = meshAsset->getMesh(0);

	mVertexBuffer = static_cast<VulkanVertexBuffer*>(mesh->getVBO());
	mIndexBuffer = static_cast<VulkanIndexBuffer*>(mesh->getIBO());

	UniformBufferCreateInfo uniformBufferCreateInfo = {};
	uniformBufferCreateInfo.flags = 0;
	uniformBufferCreateInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	uniformBufferCreateInfo.size = sizeof(UBO) * 2;
	uniformBufferCreateInfo.usage = EBufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER;

	mUniformBuffer = new VulkanUniformBuffer(mContext);
	mUniformBuffer->construct(uniformBufferCreateInfo);

	auto texAsset = AssetManager::instance().load<TextureAsset>("test", "data/textures/shawn.png", STBI_rgb_alpha);

	VulkanSampler* sampler = new VulkanSampler(mContext);
	sampler->construct({ 0, EFilter::LINEAR, EFilter::LINEAR, ESamplerMipmapMode::LINEAR, ESamplerAddressMode::REPEAT,
	ESamplerAddressMode::REPEAT , ESamplerAddressMode::REPEAT , 0.0f, false, 16.0f, false, ECompareOp::COMPARE_OP_ALWAYS,
	0, 1, EBorderColor::FLOAT_OPAQUE_BLACK, false });

	mTexture = new VulkanTexture(mContext);
	mTexture->construct({ texAsset.get(), sampler, 2, mDescriptorPool });

	mSetLayout = new VulkanDescriptorSetLayout(mContext);
	mSetLayout->construct({ 0, {mUniformBuffer->getDescriptorSetLayout(0, VK_SHADER_STAGE_VERTEX_BIT, 1), mTexture->getDescriptorSetLayout(1, VK_SHADER_STAGE_FRAGMENT_BIT, 1)} });

	DescriptorSetCreateInfo setCreateInfo = {};
	setCreateInfo.pool = mDescriptorPool;
	setCreateInfo.setLayouts.push_back(mSetLayout);
	setCreateInfo.setLayouts.push_back(mSetLayout);

	mSet = new VulkanDescriptorSet(mContext);
	mSet->construct(setCreateInfo);

	mSet2 = new VulkanDescriptorSet(mContext);
	mSet2->construct(setCreateInfo);

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
	vertexState.flags = 0;
	vertexState.bindingDescriptions = { mVertexBuffer->getBinding() };
	vertexState.attributeDescriptions = mVertexBuffer->getAttributes();

	PipelineInputAssemblyStateCreateInfo assemblyState = {};
	assemblyState.topology = PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyState.primitiveRestartEnable = false;

	Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(mWindow->width());
	viewport.height = static_cast<float>(mWindow->height());
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	Vector4i rect = {};
	rect.x = 0;
	rect.y = 0;
	rect.z = static_cast<int32_t>(mWindow->width());
	rect.w = static_cast<int32_t>(mWindow->height());

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

	PipelineDepthStencilStateCreateInfo depthState = {};
	depthState.flags = 0;
	depthState.depthTestEnable = true;
	depthState.depthWriteEnable = true;
	depthState.depthCompareOp = ECompareOp::COMPARE_OP_LESS;
	depthState.depthBoundsTestEnable = false;
	depthState.minDepthBounds = 0.0f;
	depthState.maxDepthBounds = 1.0f;
	depthState.stencilTestEnable = false;
	depthState.front = {};
	depthState.back = {};

	mLayout = new VulkanPipelineLayout(mContext);
	mLayout->construct({ 0, {mSetLayout}, {} });

	GraphicsPipelineCreateInfo createInfo = {};
	createInfo.flags = 0;
	createInfo.stages = { vertStage, fragStage };

	createInfo.vertexState = &vertexState;
	createInfo.assemblyState = &assemblyState;
	createInfo.viewportState = &viewportState;
	createInfo.rasterizationState = &rasterizationState;
	createInfo.multisampleState = &multisampleState;
	createInfo.colorBlendState = &colorBlendState;
	createInfo.layout = mLayout;
	createInfo.basePipelineHandle = nullptr;
	createInfo.basePipelineIndex = -1;
	createInfo.renderPass = mRenderPass;
	createInfo.depthStencilState = &depthState;
	createInfo.subPass = 0;

	mGraphicsPipeline->construct(createInfo);

	delete vertStage;
	delete fragStage;

	delete vertModule;
	delete fragModule;
}

void RenderSystem::preRender()
{

}

float angle = 0.0f;

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

	RenderPassRecordInfo recordInfo = {};
	recordInfo.renderPass = mRenderPass;
	recordInfo.frameBuffer = mFramebuffers[mCurrentFrame];
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

	handle->bindGraphicsPipeline(mGraphicsPipeline);

	handle->bindVertexBuffers(0, 1, { mVertexBuffer }, { 0 });
	handle->bindIndexBuffer(mIndexBuffer, 0, INDEX_TYPE_UINT16);

	std::vector<VkDescriptorSet> setList;
	VkDescriptorSet set = mSet->getHandle(mCurrentFrame);
	VkDescriptorSet set2 = mSet2->getHandle(mCurrentFrame);

	OffsetSize offset = { 0, sizeof(UBO) };
	auto uniformWriteDescHolder = mUniformBuffer->getWriteDescriptor(0, offset);
	auto uniformWriteDesc = uniformWriteDescHolder.getWriteDescriptorSet();
	uniformWriteDesc.dstSet = set;

	auto textureWriteDescHolder = mTexture->getWriteDescriptor(1, {});
	auto textureWriteDesc = textureWriteDescHolder.getWriteDescriptorSet();
	textureWriteDesc.dstSet = set;

	std::vector<VkWriteDescriptorSet> writeSets;
	writeSets.push_back(uniformWriteDesc);
	writeSets.push_back(textureWriteDesc);

	vkUpdateDescriptorSets(mContext->getDevice(), writeSets.size(), writeSets.data(), 0, nullptr);

	UBO u = {};
	u.proj = Matrix4f::perspective(glm::radians(60.0f), (static_cast<float>(mWindow->width()) / static_cast<float>(mWindow->height())), 0.001f, 1000.0f);
	u.view = Matrix4f::lookAt(Vector3f(10, 10, 10), Vector3f(0, 0, 0), Vector3f(0, 0, -1));
	u.model = Matrix4f::identity();
	u.model = Matrix4f::rotate(u.model, Vector3f(0, 1, 0), angle);

	angle += 0.0001f;

	mUniformBuffer->setData(&u, 0);

	setList.push_back(set);

	vkCmdBindDescriptorSets(handle->getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout->getHandle(), 0, 
		1, &setList[0], 0, nullptr);

	handle->drawIndexed(mIndexBuffer->getCount(), 1, 0, 0, 0);

	offset = { sizeof(UBO), sizeof(UBO) };
	uniformWriteDescHolder = mUniformBuffer->getWriteDescriptor(0, offset);
	uniformWriteDesc = uniformWriteDescHolder.getWriteDescriptorSet();
	uniformWriteDesc.dstSet = set2;

	textureWriteDescHolder = mTexture->getWriteDescriptor(1, {});
	textureWriteDesc = textureWriteDescHolder.getWriteDescriptorSet();
	textureWriteDesc.dstSet = set2;

	writeSets.clear();
	writeSets.push_back(uniformWriteDesc);
	writeSets.push_back(textureWriteDesc);

	vkUpdateDescriptorSets(mContext->getDevice(), writeSets.size(), writeSets.data(), 0, nullptr);

	u.proj = Matrix4f::perspective(glm::radians(60.0f), (static_cast<float>(mWindow->width()) / static_cast<float>(mWindow->height())), 0.001f, 1000.0f);
	u.view = Matrix4f::lookAt(Vector3f(10, 10, 10), Vector3f(0, 0, 0), Vector3f(0, 0, -1));
	u.model = Matrix4f::identity();
	u.model = Matrix4f::translate(u.model, Vector3f(15, 0, 0));

	mUniformBuffer->setData(&u, sizeof(UBO));

	setList.clear();
	setList.push_back(set2);

	vkCmdBindDescriptorSets(handle->getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout->getHandle(), 0,
		1, &setList[0], 0, nullptr);

	handle->drawIndexed(mIndexBuffer->getCount(), 1, 0, 0, 0);

	handle->endRenderPass();

	handle->end();

	mSwapChain->submit(handle);
	mSwapChain->swap();

	mCurrentFrame = (mCurrentFrame + 1) % mFlightSize;
}

void RenderSystem::postRender()
{

}

void RenderSystem::onEvent(Event& aEvent)
{
	EventDispatcher dispatcher(aEvent);
	dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(RenderSystem::_onResize));
}

bool RenderSystem::_onResize(WindowResizeEvent& aEvent) const
{
	VulkanGraphicsContext* vkContext = primal_cast<VulkanGraphicsContext*>(mContext);
	vkContext->idle();

	const uint32_t newWidth = aEvent.width();
	const uint32_t newHeight = aEvent.height();

	SwapChainCreateInfo swapChainInfo = {};
	swapChainInfo.surfaceHandle = reinterpret_cast<uint64_t>(mContext->getSurfaceHandle());
	swapChainInfo.width = newWidth;
	swapChainInfo.height = newHeight;
	swapChainInfo.maxImageCount = mFlightSize;

	mSwapChain->reconstruct(swapChainInfo);

	const CommandBufferCreateInfo commandBufferInfo =
	{
		vkContext->getCommandPool(),
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

	mRenderPass->reconstruct(renderPassInfo);

	auto views = mSwapChain->getImageViews();

	uint32_t fbc = 0;

	for (const auto view : views)
	{
		FramebufferCreateInfo frameBufferInfo = {};
		frameBufferInfo.attachments.push_back(view);
		frameBufferInfo.attachments.push_back(mSwapChain->getDepthView());
		frameBufferInfo.renderPass = mRenderPass;
		frameBufferInfo.height = mWindow->height();
		frameBufferInfo.width = mWindow->width();
		frameBufferInfo.layers = 1;

		VulkanFramebuffer* fb = mFramebuffers[fbc];
		fb->reconstruct(frameBufferInfo);
		*(mFramebuffers + fbc) = fb;

		fbc++;
	}

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		mPrimaryBuffer[i]->reconstruct(commandBufferInfo);
	}

	UniformBufferCreateInfo uniformBufferCreateInfo = {};
	uniformBufferCreateInfo.flags = 0;
	uniformBufferCreateInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	uniformBufferCreateInfo.size = sizeof(UBO);
	uniformBufferCreateInfo.usage = EBufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER;

	mUniformBuffer->reconstruct(uniformBufferCreateInfo);

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

	PipelineVertexStateCreateInfo vertexState = {};
	vertexState.flags = 0;
	vertexState.bindingDescriptions = { mVertexBuffer->getBinding() };
	vertexState.attributeDescriptions = mVertexBuffer->getAttributes();

	PipelineInputAssemblyStateCreateInfo assemblyState = {};
	assemblyState.topology = PrimitiveTopology::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyState.primitiveRestartEnable = false;

	Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(mWindow->width());
	viewport.height = static_cast<float>(mWindow->height());
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	Vector4i rect = {};
	rect.x = 0;
	rect.y = 0;
	rect.z = static_cast<int32_t>(mWindow->width());
	rect.w = static_cast<int32_t>(mWindow->height());

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

	PipelineDepthStencilStateCreateInfo depthState = {};
	depthState.flags = 0;
	depthState.depthTestEnable = true;
	depthState.depthWriteEnable = true;
	depthState.depthCompareOp = ECompareOp::COMPARE_OP_LESS;
	depthState.depthBoundsTestEnable = false;
	depthState.minDepthBounds = 0.0f;
	depthState.maxDepthBounds = 1.0f;
	depthState.stencilTestEnable = false;
	depthState.front = {};
	depthState.back = {};

	mLayout->reconstruct({ 0, {mSetLayout}, {} });

	GraphicsPipelineCreateInfo createInfo = {};
	createInfo.flags = 0;
	createInfo.stages = { vertStage, fragStage };

	createInfo.vertexState = &vertexState;
	createInfo.assemblyState = &assemblyState;
	createInfo.viewportState = &viewportState;
	createInfo.rasterizationState = &rasterizationState;
	createInfo.multisampleState = &multisampleState;
	createInfo.colorBlendState = &colorBlendState;
	createInfo.layout = mLayout;
	createInfo.basePipelineHandle = nullptr;
	createInfo.basePipelineIndex = -1;
	createInfo.renderPass = mRenderPass;
	createInfo.subPass = 0;
	createInfo.depthStencilState = &depthState;

	mGraphicsPipeline->reconstruct(createInfo);

	delete vertStage;
	delete fragStage;

	delete vertModule;
	delete fragModule;

	vkContext->idle();

	return false;
}
