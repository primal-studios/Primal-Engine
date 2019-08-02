#include "ecs/Entity.h"
#include "ecs/EntityManager.h"

#include "components/TransformComponent.h"

Entity::Entity(const std::string& aName)
{
	mParent = nullptr;
	transform = nullptr;
	mName = aName;
	mManager = nullptr;
}

Entity::~Entity()
{
	for(const auto& comp : mComponents)
	{
		delete comp;
	}
	
	mComponents.clear();
}

const std::string& Entity::name() const
{
	return mName;
}

void Entity::setParent(Entity* aParent)
{
	mParent = aParent;

	aParent->children.push_back(this);
}

Entity* Entity::parent() const
{
	return mParent;
}

void Entity::_removeComponent(Component* aComponent)
{
	const auto iter = std::find(mComponents.begin(), mComponents.end(), aComponent);
	if(iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

void* Entity::_getNewComponentMemoryBlock() const
{
	return mManager->mComponentPool->getBlock();
}

void Entity::_addComponent(const std::type_index aIndex, Component* aComponent) const
{
	mManager->mComponentTypeMap[aIndex].push_back(aComponent);
}
