#ifndef entitymanager_h__
#define entitymanager_h__

#include <string>
#include <unordered_map>
#include <typeindex>

#include "Entity.h"
#include "events/Event.h"
#include "core/PoolAllocator.h"
#include "ecs/ComponentView.h"

class EntityManager
{
	friend class Entity;
	friend class Component;
	public:
		using EventCallbackFunction = std::function<void(Event&)>;
		static EntityManager& instance();
		
		Entity* create(const std::string& aName = "Entity");
		Entity* get(const std::string& aName);

		template<typename T>
		ComponentView<T> getComponentsByType();

		void destroy(Entity* aEntity);
		void destroyAll() const;

		void setEventCallback(const EventCallbackFunction& aCallback);
		EventCallbackFunction getEventCallback() const { return mCallback; }

	private:
		EntityManager();

		std::unordered_map<std::string, Entity*> mEntityMap;
		std::unordered_map<std::type_index, std::vector<Component*>> mComponentTypeMap;

		PoolAllocator* mEntityPool;
		PoolAllocator* mComponentPool;

		std::vector<Component*> mEmptyVector;

		EventCallbackFunction mCallback;
};

template <typename T>
ComponentView<T> EntityManager::getComponentsByType()
{
	auto iter = mComponentTypeMap.find(std::type_index(typeid(T)));
	if(iter != mComponentTypeMap.end())
	{
		std::vector<Component*>& temp = iter->second;
		return ComponentView<T>(temp);
	}

	return ComponentView<T>(mEmptyVector);
}

#endif // entitymanager_h__
