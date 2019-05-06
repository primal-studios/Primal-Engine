#ifndef entity_h__
#define entity_h__

#include <string>
#include <list>

#include "ecs/Component.h"
#include <typeindex>

class EntityManager;
class Entity
{
	friend class EntityManager;
	friend class Component;
	public:
		explicit Entity(const std::string& aName = "Entity");
		~Entity();

		template<typename T, typename ... Arguments>
		T* addComponent(Arguments&& ... aArgs);

		template<typename T>
		T* getComponent();

		template<typename T>
		std::list<T*> getComponents();

		std::list<Component*>::iterator begin() { return mComponents.begin(); }
		std::list<Component*>::iterator end() { return mComponents.end(); }

		const std::string& name() const;

	private:
		std::list<Component*> mComponents;
		std::string mName;

		EntityManager* mManager;

		void _removeComponent(Component* aComponent);
		void* _getNewComponentMemoryBlock() const;

		void _addComponent(const std::type_index aIndex, Component* aComponent) const;
};

template <typename T, typename ... Arguments>
T* Entity::addComponent(Arguments&&... aArgs)
{
	static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

	T* component = static_cast<T*>(_getNewComponentMemoryBlock());
	::new(component) T(std::forward<Arguments>(aArgs)...);

	component->entity = this;
	component->onConstruct();

	mComponents.push_back(component);
	_addComponent(std::type_index(typeid(T)), component);

	return component;
}

template <typename T>
T* Entity::getComponent()
{
	static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

	for (const auto& comp : mComponents)
	{
		if (auto check = dynamic_cast<T>(comp))
		{
			return check;
		}
	}
	
	return nullptr;
}

template <typename T>
std::list<T*> Entity::getComponents()
{
	static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

	std::list<T> components;

	for(const auto& comp : mComponents)
	{
		if(auto check = dynamic_cast<T>(comp))
		{
			components.push_back(comp);
		}
	}

	return components;
}

#endif // entity_h__
