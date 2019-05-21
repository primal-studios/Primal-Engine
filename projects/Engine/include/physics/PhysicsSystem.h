#ifndef physicssystem_h__
#define physicssystem_h__

#include "ecs/System.h"

#include <physx/PxPhysicsAPI.h>

class PhysicsSystem final : public System
{
	friend class RigidBody;
	friend class StaticBody;
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() = default;

		void initialize() override;

		void fixedUpdate() override;

		void dispose() override;

	private:
		physx::PxFoundation* mFoundation;
		physx::PxPhysics* mPhysics;
		physx::PxPvd* mPvd;
		physx::PxPvdTransport* mTransport;
		physx::PxCooking* mCooking;

		physx::PxDefaultCpuDispatcher* mCpuDispatcher;

		physx::PxScene* mScene;

		physx::PxDefaultAllocator* mDefaultAllocator;
		physx::PxDefaultErrorCallback* mDefaultErrorCallback;
};

#endif // physicssystem_h__