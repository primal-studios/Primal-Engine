#include "systems/RenderSystem.h"

#include "core/PrimalCast.h"
#include "ecs/EntityManager.h"
#include "filesystem/FileSystem.h"
#include "graphics/MaterialManager.h"
#include "graphics/vk/VulkanShaderModule.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanVertexBuffer.h"
#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanDescriptorPool.h"

#include <stb/stb_image.h>

#include "assets/AssetManager.h"
#include "assets/MeshAsset.h"
#include "assets/ShaderAsset.h"
#include "assets/TextureAsset.h"
#include "components/MeshRenderComponent.h"
#include "components/MeshContainerComponent.h"
#include "graphics/RenderPassManager.h"

RenderSystem::RenderSystem(Window* aWindow)
	: mRenderPass(nullptr), mGraphicsPipeline(nullptr), mVertexBuffer(nullptr), mIndexBuffer(nullptr),
	  mUboPool(nullptr),
	  mTexture(nullptr), mWindow(aWindow)
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
	swapChainInfo.windowHandle = reinterpret_cast<uint64_t>(aWindow->getNativeWindow());
	swapChainInfo.maxImageCount = mFlightSize;

	mSwapChain->construct(swapChainInfo);

	std::vector<DescriptorPoolSize> poolSizes;
	DescriptorPoolSize uniformBufferSize{};
	uniformBufferSize.type = EDescriptorType::UNIFORM_BUFFER_DYNAMIC;
	uniformBufferSize.count = 4;

	DescriptorPoolSize combinedSamplerSize{};
	combinedSamplerSize.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
	combinedSamplerSize.count = 2;

	poolSizes.push_back(combinedSamplerSize);

	DescriptorPoolCreateInfo createInfo;
	createInfo.flags = 0;
	createInfo.maxSets = 4;
	createInfo.poolSizes = poolSizes;

	mDescPool = new DescriptorSetPool(2, createInfo);
}

RenderSystem::~RenderSystem()
{
	vkDeviceWaitIdle(primal_cast<VulkanGraphicsContext*>(mContext)->getDevice());
	
	AssetManager::instance().unloadAll();

	delete mSwapChain;

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		delete mFramebuffers[i];
	}

	for (auto& frame : mPrimaryBuffers)
	{
		for (auto& buf : frame)
		{
			delete buf.second;
		}
	}

	for (auto& frame : mSecondaryBuffers)
	{
		for (auto& pass : frame)
		{
			for (auto& buf : pass.second)
			{
				delete buf;
			}
		}
	}

	delete[] mFramebuffers;

	delete mCpyBuffer;
	delete mSceneData;

	MaterialManager::instance().reset();

	delete mDescPool;

	{
		mShaderAsset = nullptr;
		mRenderPassAsset = nullptr;
	}

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
	mPrimaryBuffers.resize(mFlightSize);
	mSecondaryBuffers.resize(mFlightSize);

	const CommandBufferCreateInfo commandBufferInfo =
	{
		mContext->getCommandPool(),
		true
	};

	const CommandBufferCreateInfo transferBufferInfo =
	{
		mContext->getTransferCommandPool(),
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

	mRenderPassAsset = AssetManager::instance().load<RenderPassAsset>("defaultRenderpass", "data/renderpasses/default.json");
	mRenderPass = mRenderPassAsset->getRenderPass();

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

	mShaderAsset = AssetManager::instance().load<ShaderAsset>("testShader", "data/effects/default.json");
	mGraphicsPipeline = mShaderAsset->getPipeline();
	mLayout = primal_cast<VulkanPipelineLayout*>(mGraphicsPipeline->getCreateInfo().layout);

	auto meshAsset = AssetManager::instance().load<MeshAsset>("mesh", "data/models/cube.glb");
	auto mesh = meshAsset->getMesh(0);

	mVertexBuffer = primal_cast<VulkanVertexBuffer*>(mesh->getVBO());
	mIndexBuffer = primal_cast<VulkanIndexBuffer*>(mesh->getIBO());

	mCpyBuffer = new VulkanCommandBuffer(mContext);
	mCpyBuffer->construct(transferBufferInfo);
	mCpyReady = 2;

	CommandBufferRecordInfo recordInfo = {};
	recordInfo.flags = COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE;
	mCpyBuffer->record(recordInfo);
	mCpyBuffer->copyBuffers(mSwapChain, mVertexBuffer, mesh->getData(), mesh->getSize());
	mCpyBuffer->copyBuffers(mSwapChain, mIndexBuffer, mesh->getIndices(), mesh->getIndicesSize());
	mCpyBuffer->end();

	UniformBufferCreateInfo uniformBufferCreateInfo = {};
	uniformBufferCreateInfo.flags = 0;
	uniformBufferCreateInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	uniformBufferCreateInfo.size = 65536;
	uniformBufferCreateInfo.usage = EBufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER;
	
	UniformBufferObjectElement* modl = new UniformBufferObjectElement{
		"model",
		EUniformBufferObjectElementType::UBO_TYPE_MAT4,
		0,
		sizeof(UBO::model)
	};

	UniformBufferObjectElement* view = new UniformBufferObjectElement{
		"view",
		EUniformBufferObjectElementType::UBO_TYPE_MAT4,
		0,
		sizeof(UBO::view)
	};

	UniformBufferObjectElement* proj = new UniformBufferObjectElement{
		"proj",
		EUniformBufferObjectElementType::UBO_TYPE_MAT4,
		64,
		sizeof(UBO::proj)
	};

	std::vector<UniformBufferObjectElement*> elements = { modl };

	mUboPool = new UniformBufferPool(65536 / sizeof(UBO), sizeof(UBO), 0, uniformBufferCreateInfo, elements);

	auto texAsset = AssetManager::instance().load<TextureAsset>("test", "data/textures/Shawn.json", STBI_rgb_alpha);
	mTexture = primal_cast<VulkanTexture*>(texAsset->getTexture());

	auto tex2 = AssetManager::instance().load<TextureAsset>("test2", "data/textures/Test.json", STBI_rgb_alpha);

	mGraphicsPipeline = mShaderAsset->getPipeline();
	mSceneData = new SceneData({ {view, proj}, mShaderAsset->getLayout() });
}

void RenderSystem::preRender()
{
	// DO ALL COPIES
}

float angle = 0.0f;

void RenderSystem::render()
{
	for (const auto& renderPass : mRenderMap)
	{
		const auto handle = primal_cast<VulkanCommandBuffer*>(mPrimaryBuffers[mCurrentFrame][renderPass.first]);
		const auto prevHandle = mPrimaryBuffers[(mCurrentFrame - 1) % mFlightSize][renderPass.first];

		CommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.frameBuffer = primal_cast<VulkanRenderPass*>(renderPass.first)->getFramebuffer();
		inheritanceInfo.renderPass = renderPass.first;

		CommandBufferRecordInfo recordInfo = {};
		recordInfo.inheritance = &inheritanceInfo;
		recordInfo.flags = COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE;

		handle->record(recordInfo);

		for (const auto& layouts : renderPass.second)
		{
			// Set the scene information for the layout
			for (const auto& pipeline : layouts.second)
			{
				const auto secondary = primal_cast<VulkanCommandBuffer*>(mSecondaryBuffers[mCurrentFrame][renderPass.first][pipeline.first->getSubpass()]);

				CommandBufferInheritanceInfo secondaryInheritanceInfo = {};
				secondaryInheritanceInfo.renderPass = renderPass.first;
				secondaryInheritanceInfo.frameBuffer = primal_cast<VulkanRenderPass*>(renderPass.first)->getFramebuffer();
				secondaryInheritanceInfo.occlusionQueryEnable = false;
				secondaryInheritanceInfo.queryPrecsise = false;
				secondaryInheritanceInfo.subPass = pipeline.first->getSubpass();
				secondaryInheritanceInfo.pipelineStatistics = 0;

				CommandBufferRecordInfo secondaryRecordInfo = {};
				secondaryRecordInfo.inheritance = &secondaryInheritanceInfo;
				secondaryRecordInfo.flags = COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE;

				secondary->record(secondaryRecordInfo);

				for (const auto& mesh : pipeline.second)
				{
					// bind mesh
					for (const auto& mat : mesh.second)
					{
						// set material info
						for (const auto& instance : mat.second)
						{
							// draw here
						}
					}
				}

				secondary->end();
			}
		}

		handle->executeCommands(mSecondaryBuffers[mCurrentFrame][renderPass.first]);

		handle->end();
	}

//	const auto handle = *(mPrimaryBuffer + mCurrentFrame);
//	const auto prevHandle = *(mPrimaryBuffer + ((mCurrentFrame - 1) % mFlightSize));
//
//	mPrimaryInheritance.renderPass = mRenderPass;
//	mPrimaryInheritance.frameBuffer = mFramebuffers[mCurrentFrame];
//	mPrimaryInheritance.occlusionQueryEnable = false;
//	mPrimaryInheritance.queryPrecsise = false;
//	mPrimaryInheritance.subPass = 0;
//	mPrimaryInheritance.pipelineStatistics = 0;
//
//	mPrimaryRecordInfo.inheritance = &mPrimaryInheritance;
//	mPrimaryRecordInfo.flags = COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE;
//
//	mSwapChain->beginFrame();
//
//	if (mCpyReady == 2) {
//		handle->addDependency(mCpyBuffer);
//		mSwapChain->submitToTransferQueue(mCpyBuffer);
//		mCpyReady = 1;
//	}
//	else if (mCpyReady == 1) {
//		prevHandle->removeDependency(mCpyBuffer);
//		mCpyReady = 0;
//	}
//	
//	handle->record(mPrimaryRecordInfo);
//
//	RenderPassRecordInfo recordInfo = {};
//	recordInfo.renderPass = mRenderPass;
//	recordInfo.frameBuffer = mFramebuffers[mCurrentFrame];
//	recordInfo.renderArea = { 0, 0, static_cast<int32_t>(mWindow->width()), static_cast<int32_t>(mWindow->height()) };
//	
//	ClearValue clear = {};
//	clear.color.float32[0] = 0.0f;
//	clear.color.float32[1] = 0.0f;
//	clear.color.float32[2] = 0.0f;
//	clear.color.float32[3] = 0.0f;
//
//	ClearValue depth = {};
//	depth.depthStencil = { 1.0f, 0 };
//	recordInfo.clearValues.push_back(clear);
//	recordInfo.clearValues.push_back(depth);
//
//	handle->recordRenderPass(recordInfo);
//
//	handle->bindGraphicsPipeline(mGraphicsPipeline);
//
//	handle->bindVertexBuffers(0, 1, { mVertexBuffer }, { 0 });
//	handle->bindIndexBuffer(mIndexBuffer, 0, INDEX_TYPE_UINT16);
//
//	UBO u = {};
//	u.proj = Matrix4f::perspective(glm::radians(60.0f), (static_cast<float>(mWindow->width()) / static_cast<float>(mWindow->height())), 0.001f, 1000.0f);
//	u.view = Matrix4f::lookAt(Vector3f(40, 0, 40), Vector3f(0, 0, 0), Vector3f(0, 0, -1));
//	u.model = Matrix4f::identity();
//	u.model = Matrix4f::rotate(u.model, Vector3f(1, 1, 0), angle);
//
//	angle += 0.0001f;
//
//	mMaterialInstance->setVariable<Matrix4f>("model", u.model);
//
//	u.model = Matrix4f::identity();
//	u.model = Matrix4f::translate(u.model, Vector3f(0, 20, 0));
//	mMaterialInstance2->setVariable<Matrix4f>("model", u.model);
//
//	mSceneData->setValue("proj", u.proj);
//	mSceneData->setValue("view", u.view);
//
//	handle->bindSceneData(mSceneData, mCurrentFrame); // bind once per pipeline layout. More than once is invalid
//
//	handle->bindMaterial(mMaterialInstance->getParentMaterial(), mCurrentFrame);
//	handle->bindMaterialInstance(mMaterialInstance, mCurrentFrame);
//	handle->drawIndexed(mIndexBuffer->getCount(), 1, 0, 0, 0);
//
//	handle->bindMaterial(mMaterialInstance2->getParentMaterial(), mCurrentFrame);
//	handle->bindMaterialInstance(mMaterialInstance2, mCurrentFrame);
//	handle->drawIndexed(mIndexBuffer->getCount(), 1, 0, 0, 0);
//
//	for (uint32_t i = 0; i < mInstances.size(); i++)
//	{
//		uint32_t x = i - static_cast<float>(mInstances.size()) / 2.0f;
//		Matrix4f modl = Matrix4f::identity();
//		modl = Matrix4f::translate(modl, Vector3f(-5, static_cast<float>(i) * 10, 5));
//		mInstances[i]->setVariable("model", modl);
//		handle->bindMaterialInstance(mInstances[i], mCurrentFrame);
//		handle->drawIndexed(mIndexBuffer->getCount(), 1, 0, 0, 0);
//	}
//
//	handle->endRenderPass();
//
//	handle->end();
//
//	mSwapChain->submit(handle);
//	mSwapChain->swap();

	mCurrentFrame = (mCurrentFrame + 1) % mFlightSize;
}

void RenderSystem::postRender()
{

}

void RenderSystem::onEvent(Event& aEvent)
{
	EventDispatcher dispatcher(aEvent);
	dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FUNCTION(RenderSystem::_onResize));
	dispatcher.dispatch<ComponentAddedEvent<MeshRenderComponent>>(BIND_EVENT_FUNCTION(RenderSystem::_onMeshRenderComponentAdded));
}

bool RenderSystem::_onResize(WindowResizeEvent& aEvent) const
{
	int32_t width = 0, height = 0;

	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(mWindow->getNativeWindow(), &width, &height);
		glfwWaitEvents();
	}

	return false;
}

bool RenderSystem::_onMeshRenderComponentAdded(ComponentAddedEvent<MeshRenderComponent>& aEvent)
{
	MeshRenderComponent* renderComponent = primal_cast<MeshRenderComponent*>(aEvent.getComponent());
	MeshContainerComponent* meshComponent = renderComponent->mContainer;

	DEBUG_ONLY_BLOCK({
		PRIMAL_ASSERT(renderComponent->entity == meshComponent->entity, "Entity of components does not match.");
	});

	MaterialInstance* matInstance = renderComponent->mMaterialInstance;
	Material* material = matInstance->getParentMaterial();
	IGraphicsPipeline* pipeline = material->getPipeline();
	IPipelineLayout* layout = pipeline->getLayout();
	IRenderPass* renderPass = pipeline->getRenderPass();
	const Mesh* mesh = meshComponent->getMesh();

	if (mRenderMap.find(renderPass) == mRenderMap.end())
	{
		for (size_t i = 0; i < mFlightSize; i++)
		{
			auto& renderPasses = mPrimaryBuffers[i];
			VulkanCommandBuffer* buf = new VulkanCommandBuffer(mContext);

			CommandBufferCreateInfo commandBufferInfo =
			{
				mContext->getCommandPool(),
				true
			};

			buf->construct(commandBufferInfo);

			renderPasses[renderPass] = buf;
		}

		for (size_t i = 0; i < mFlightSize; i++)
		{
			auto& renderPasses = mSecondaryBuffers[i];
			VulkanCommandBuffer* buf = new VulkanCommandBuffer(mContext);

			CommandBufferCreateInfo commandBufferInfo =
			{
				mContext->getCommandPool(),
				false
			};

			buf->construct(commandBufferInfo);

			auto& pipelines = renderPasses[renderPass];
			if (pipelines.size() <= pipeline->getSubpass())
			{
				pipelines.resize(pipeline->getSubpass() + 1);
			}
			pipelines[pipeline->getSubpass()] = buf;
		}
	}
	else if (mRenderMap[renderPass].find(layout) == mRenderMap[renderPass].end())
	{
		if (mRenderMap[renderPass][layout].find(pipeline) == mRenderMap[renderPass][layout].end())
		{
			for (size_t i = 0; i < mFlightSize; i++)
			{
				auto& renderPasses = mSecondaryBuffers[i];
				VulkanCommandBuffer* buf = new VulkanCommandBuffer(mContext);

				CommandBufferCreateInfo commandBufferInfo =
				{
					mContext->getCommandPool(),
					false
				};

				buf->construct(commandBufferInfo);

				auto pipelines = renderPasses[renderPass];
				if (pipelines.size() <= pipeline->getSubpass())
				{
					pipelines.resize(pipeline->getSubpass() + 1);
				}
				pipelines[pipeline->getSubpass()] = buf;
			}
		}
	}

	auto& layouts = mRenderMap[renderPass];
	auto& pipelines = layouts[layout];
	auto& meshes = pipelines[pipeline];
	auto& materials = meshes[mesh];
	auto& materialInstances = materials[material];

	DEBUG_ONLY_BLOCK({
		PRIMAL_ASSERT(std::find(materialInstances.begin(), materialInstances.end(), renderComponent) == materialInstances.end(), "Render map already contains MeshRenderComponent provided.")
	});

	materialInstances.push_back(renderComponent);

	return false;
}
