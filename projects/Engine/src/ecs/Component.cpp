#include "ecs/Component.h"
#include "ecs/Entity.h"
#include "ecs/EntityManager.h"

void Component::destroy(Component* aComponent, const bool aRemoveFromEntity)
{
	if(aRemoveFromEntity)
		aComponent->entity->_removeComponent(aComponent);
	
	EntityManager::instance().mComponentPool->freeBlock(static_cast<void*>(aComponent));
	aComponent = nullptr;
}
