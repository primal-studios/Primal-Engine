#include "ecs/EntityManager.h"
#include "ecs/Entity.h"

#include "core/Log.h"

#include "components/TransformComponent.h"

EntityManager& EntityManager::instance()
{
	static EntityManager* instance = new EntityManager();
	return *instance;
}

Entity* EntityManager::create(const std::string& aName)
{
	if(mEntityMap.size() > 0)
	{
		if(mEntityMap.find(aName) != mEntityMap.end())
		{
			PRIMAL_INTERNAL_ERROR("Entity already exists with name {0}", aName);
			return nullptr;
		}
	}

	Entity* entity = static_cast<Entity*>(mEntityPool->getBlock());
	::new(entity) Entity(aName);

	entity->mManager = this;
	entity->addComponent<TransformComponent>();

	return entity;
}

Entity* EntityManager::get(const std::string& aName)
{
	if (mEntityMap.size() > 0)
	{
		const auto iter = mEntityMap.find(aName);
		if (iter != mEntityMap.end())
		{
			return iter->second;
		}
	}

	return nullptr;
}

void EntityManager::destroy(Entity* aEntity)
{
	if (!aEntity)
		return;

	const auto iter = mEntityMap.find(aEntity->name());
	if(iter != mEntityMap.end())
	{
		mEntityMap.erase(iter);

		mEntityPool->freeBlock(static_cast<void*>(aEntity));
		aEntity = nullptr;
	}
}

void EntityManager::destroyAll() const
{
	delete mEntityPool;
	delete mComponentPool;
}

EntityManager::EntityManager()
{
	mEntityPool = new PoolAllocator(64, 1000000, 512);
	mComponentPool = new PoolAllocator(64, 1000000, 512);
}
