#include "ecs/Component.h"
#include "ecs/Entity.h"
#include "ecs/EntityManager.h"

Component::~Component()
{
	entity->_removeComponent(this);
	EntityManager::instance().mComponentPool->freeBlock(static_cast<void*>(this));
}
