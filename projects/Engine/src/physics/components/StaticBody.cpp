#include "physics/components/StaticBody.h"

#include "ecs/Entity.h"
#include "ecs/SystemManager.h"
#include "physics/PhysicsSystem.h"

StaticBody::StaticBody()
{
	mBody = nullptr;
}

StaticBody::~StaticBody()
{
	const auto physics = SystemManager::instance().getSystem<PhysicsSystem>();
	physics->mScene->removeActor(*mBody);
	mBody->release();
	mBody = nullptr;
}

void StaticBody::onConstruct()
{
	const auto physics = SystemManager::instance().getSystem<PhysicsSystem>();
	const Vector3f position = *entity->transform->position;
	mBody = physics->mPhysics->createRigidStatic(physx::PxTransform(physx::PxVec3(position.x, position.y, position.z)));

	physics->mScene->addActor(*mBody);
}

void StaticBody::addCollider(Collider* aCollider)
{
	mColliders.push_back(aCollider);
}

void StaticBody::_removeCollider(Collider* aCollider)
{
	const auto iter = std::find(mColliders.begin(), mColliders.end(), aCollider);
	mColliders.erase(iter);
}
