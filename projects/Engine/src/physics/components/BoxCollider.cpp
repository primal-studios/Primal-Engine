#include "physics/components/BoxCollider.h"
#include <physx/extensions/PxRigidActorExt.h>
#include <physx/PxRigidStatic.h>

BoxCollider::BoxCollider(const Vector3f& aSize)
{
	physx::PxRigidStatic* actor = nullptr;
	physx::PxMaterial* material = nullptr;
	const auto geom = physx::PxBoxGeometry(aSize.x / 2, aSize.y / 2, aSize.z / 2);
	physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, 
		geom, &material, 1);
}

BoxCollider::~BoxCollider()
{
	
}