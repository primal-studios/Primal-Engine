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
}

RenderSystem::~RenderSystem()
{
	delete mPool;
	delete mContext;
}

void RenderSystem::initialize()
{

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
