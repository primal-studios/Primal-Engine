#ifndef componentevent_h__
#define componentevent_h__

#include <typeindex>

#include "events/Event.h"
#include "ecs/Component.h"
#include "core/Property.h"

template<typename T>
class ComponentAddedEvent final : public Event
{
	public:
		explicit ComponentAddedEvent(Component* aComponent)
			: mType(std::type_index(typeid(T)))
		{
			static_assert(std::is_base_of_v<Component, T>, "Component is not base of T.");

			mComponent = aComponent;
		}

		std::string toString() const override
		{
			return std::to_string(mType.hash_code());
		}

		Component* getComponent() { return mComponent; }
		const Component* getComponent() const { return mComponent; }

		std::type_index getComponentType() const { return mType; }

		EVENT_CLASS_TYPE(ComponentAddedEvent)

	private:
		std::type_index mType;
		Component* mComponent;
};

template<typename T>
class ComponentRemovedEvent final : public Event
{
	public:
		explicit ComponentRemovedEvent(Component* aComponent)
			: mType(std::type_index(typeid(T)))
		{
			static_assert(std::is_base_of_v<Component, T>, "Component is not base of T.");

			mComponent = aComponent;
		}

		std::string toString() const override
		{
			return std::to_string(mType.hash_code());
		}

		Component* getComponent() { return mComponent; }
		const Component* getComponent() const { return mComponent; }

		std::type_index getComponentType() const { return mType; }

		EVENT_CLASS_TYPE(ComponentRemovedEvent)

	private:
		std::type_index mType;
		Component* mComponent;
};

#endif // componentevent_h__