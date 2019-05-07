#ifndef entity_h__
#define entity_h__

#include <string>
#include <vector>
#include <typeindex>

#include "ecs/Component.h"
#include "components/TransformComponent.h"

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
		std::vector<T*> getComponents();

		template<typename T>
		T* getComponentInChildren();

		template<typename T>
		std::vector<T*> getComponentsInChildren();

		std::vector<Component*>::iterator begin() { return mComponents.begin(); }
		std::vector<Component*>::iterator end() { return mComponents.end(); }

		const std::string& name() const;

		TransformComponent* transform;

		void setParent(Entity* aParent);
		Entity* parent() const;

		std::vector<Entity*> children;

	private:
		std::vector<Component*> mComponents;
		std::string mName;

		Entity* mParent;

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
		if (auto check = dynamic_cast<T*>(comp))
		{
			return check;
		}
	}
	
	return nullptr;
}

template <typename T>
std::vector<T*> Entity::getComponents()
{
	static_assert(std::is_base_of<Component, T>::value, "T is not derived from Component");

	std::vector<T> components;

	for(const auto& comp : mComponents)
	{
		if(auto check = dynamic_cast<T*>(comp))
		{
			components.push_back(comp);
		}
	}

	return components;
}

template <typename T>
T* Entity::getComponentInChildren()
{
	if(getComponent<T>() != nullptr)
	{
		return getComponent<T>();
	}

	for(const auto& child : children)
	{
		if(child->getComponent<T>() != nullptr)
		{
			return child->getComponent<T>();
		}

		if(child->getComponentInChildren<T>() != nullptr)
		{
			return child->getComponentInChildren<T>();
		}
	}

	return nullptr;
}

template <typename T>
std::vector<T*> Entity::getComponentsInChildren()
{
	// TODO (Roderick): Implement this
	return std::vector<T*>();
}

#endif // entity_h__
