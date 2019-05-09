#include "systems/RenderSystem.h"

#include "ecs/EntityManager.h"
#include "components/MeshRenderComponent.h"
#include "graphics/vk/VkGraphicsContext.h"

RenderSystem::RenderSystem(Window* aWindow)
{
	GraphicsContextCreateInfo info;
	info.applicationName = "Sandbox";
	info.versionMajor = 0;
	info.versionMinor = 0;
	info.window = aWindow->getNativeWindow();

	mContext = new VkGraphicsContext(info);
}

RenderSystem::~RenderSystem()
{
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
