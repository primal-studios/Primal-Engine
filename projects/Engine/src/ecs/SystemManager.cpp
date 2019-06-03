#include "ecs/SystemManager.h"

SystemManager& SystemManager::instance()
{
	static SystemManager* instance = new SystemManager();
	return *instance;
}

void SystemManager::configure()
{
	for(const auto& system : mSystems)
	{
		system->initialize();
	}
}

void SystemManager::update()
{
	for (const auto& system : mSystems)
	{
		system->update();
	}

	for (const auto& system : mSystems)
	{
		system->lateUpdate();
	}
}

void SystemManager::fixedUpdate()
{
	for (const auto& system : mSystems)
	{
		system->fixedUpdate();
	}
}

void SystemManager::render()
{
	for (const auto& system : mSystems)
	{
		system->preRender();
	}

	for (const auto& system : mSystems)
	{
		system->render();
	}

	for (const auto& system : mSystems)
	{
		system->postRender();
	}
}

void SystemManager::dispatchEvent(Event& aEvent)
{
	for (const auto& system : mSystems)
	{
		system->onEvent(aEvent);
	}
}
