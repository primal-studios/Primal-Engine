#include "systems/RenderSystem.h"

#include "ecs/EntityManager.h"

RenderSystem::RenderSystem(Window* aWindow)
{
	GraphicsContextCreateInfo info;
	info.applicationName = "Sandbox";
	info.versionMajor = 0;
	info.versionMinor = 0;
	info.window = aWindow->getNativeWindow();

	mContext = new VulkanGraphicsContext(info);

	const CommandPoolCreateInfo commandPoolInfo = {
		0,
		mContext->getGraphicsQueueIndex()
	};

	mPool = new VulkanCommandPool(mContext);
	mPool->construct(commandPoolInfo);

	mSwapChain = new VulkanSwapChain(mContext);

	SwapChainCreateInfo swapChainInfo = {};
	swapChainInfo.surfaceHandle = reinterpret_cast<uint64_t>(mContext->getSurfaceHandle());
	swapChainInfo.width = aWindow->width();
	swapChainInfo.height = aWindow->height();

	mSwapChain->construct(swapChainInfo);
}

RenderSystem::~RenderSystem()
{
	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		delete mPrimaryBuffer[i];
	}

	delete[] mPrimaryBuffer;

	delete mSwapChain;
	delete mPool;
	delete mContext;
}

void RenderSystem::initialize()
{
	mPrimaryBuffer = new VulkanCommandBuffer*[mFlightSize];

	const CommandBufferCreateInfo commandBufferInfo = 
	{
		mPool,
		true
	};

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		mPrimaryBuffer[i] = new VulkanCommandBuffer(mContext);
	}

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		const auto nextFrame = (i + 1) % mFlightSize;
		const auto handle = *(mPrimaryBuffer + nextFrame);
		mPrimaryBuffer[i]->addDependency(handle);
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

}

void RenderSystem::postRender()
{

}
