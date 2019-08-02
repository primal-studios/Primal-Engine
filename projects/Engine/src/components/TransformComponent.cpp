#include "components/TransformComponent.h"
#include "ecs/EntityManager.h"

TransformComponent::~TransformComponent()
{
	ComponentRemovedEvent<TransformComponent> e(this);
	EntityManager::instance().getEventCallback()(e);
}
