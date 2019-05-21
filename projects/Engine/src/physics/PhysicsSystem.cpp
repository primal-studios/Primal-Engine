#include "physics/PhysicsSystem.h"
#include "core/PrimalAssert.h"

#include <physx/PxPhysicsVersion.h>

#include <physx/PxSceneDesc.h>

static physx::PxFilterFlags sCollisionFilterShader(
	const physx::PxFilterObjectAttributes aAttributes0, const physx::PxFilterData aFilterData0,
	const physx::PxFilterObjectAttributes aAttributes1, const physx::PxFilterData aFilterData1,
	physx::PxPairFlags& aPairFlags, const void* aConstantBlock, physx::PxU32 aConstantBlockSize)
{
	if (physx::PxFilterObjectIsTrigger(aAttributes0) || physx::PxFilterObjectIsTrigger(aAttributes1))
	{
		aPairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	
	aPairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	if ((aFilterData0.word0 & aFilterData1.word1) && (aFilterData1.word0 & aFilterData0.word1))
		aPairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return physx::PxFilterFlag::eDEFAULT;
}

void PhysicsSystem::initialize()
{
	mDefaultAllocator = new physx::PxDefaultAllocator();
	mDefaultErrorCallback = new physx::PxDefaultErrorCallback();

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *mDefaultAllocator, *mDefaultErrorCallback);

	PRIMAL_ASSERT(mFoundation != nullptr, "Failed to create PhysX Foundation!");

	mPvd = physx::PxCreatePvd(*mFoundation);
	mTransport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*mTransport, physx::PxPvdInstrumentationFlag::eALL);

	PRIMAL_ASSERT(mPvd != nullptr, "Failed to create PhysX PVD!");

	const physx::PxTolerancesScale scale;
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, scale, true, mPvd);

	PRIMAL_ASSERT(mPhysics != nullptr, "Failed to create PhysX Physics!");

	physx::PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = physx::PxMeshPreprocessingFlags(physx::PxMeshPreprocessingFlag::eWELD_VERTICES);

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, params);

	PRIMAL_ASSERT(mCooking != nullptr, "Failed to create PhysX Cooking!");

	const bool result = PxInitExtensions(*mPhysics, mPvd);
	PRIMAL_ASSERT(result, "Failed to initialize PhysX Extension!");

	physx::PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	if(!sceneDesc.cpuDispatcher)
	{
		mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

#if defined(PX_SUPPORT_GPU_PHYSX) && defined(WIN32)
	// TODO: Implement
#endif 

	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD;
	sceneDesc.filterShader = sCollisionFilterShader;

	PRIMAL_ASSERT(sceneDesc.isValid(), "Physx SceneDesc is not valid!");

	mScene = mPhysics->createScene(sceneDesc);
	mScene->simulate(0.02f);
}

void PhysicsSystem::fixedUpdate()
{
	
}

void PhysicsSystem::dispose()
{
	if (mPhysics)
	{
		mPhysics->release();
		mPhysics = nullptr;
	}

	if (mCooking)
	{
		mCooking->release();
		mCooking = nullptr;
	}

	if (mPvd)
	{
		mPvd->release();
		mPvd = nullptr;
	}

	if (mTransport)
	{
		mTransport->release();
		mTransport = nullptr;
	}

	if (mFoundation)
	{
		mFoundation->release();
		mFoundation = nullptr;
	}

	delete mDefaultAllocator;
	delete mDefaultErrorCallback;
}


