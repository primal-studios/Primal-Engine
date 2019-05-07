#include "systems/RenderSystem.h"

#include "ecs/EntityManager.h"
#include "components/MeshRenderComponent.h"

RenderSystem::RenderSystem()
{

}

RenderSystem::~RenderSystem()
{

}

void RenderSystem::initialize()
{

}

void RenderSystem::preRender()
{
	auto components = EntityManager::instance().getComponentsByType<MeshRenderComponent>();
	for (const auto& comp : components)
	{
		comp->onPreRender();
	}
}

void RenderSystem::render()
{
	auto components = EntityManager::instance().getComponentsByType<MeshRenderComponent>();
	for(const auto& comp : components)
	{
		comp->onRender();
	}
}

void RenderSystem::postRender()
{
	auto components = EntityManager::instance().getComponentsByType<MeshRenderComponent>();
	for (const auto& comp : components)
	{
		comp->onPostRender();
	}
}
