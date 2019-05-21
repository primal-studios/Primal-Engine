#ifndef staticbody_h__
#define staticbody_h__

#include <vector>

#include <physx/PxRigidStatic.h>

#include "ecs/Component.h"
#include "physics/components/Collider.h"

class StaticBody final : public Component
{
	public:
		StaticBody();
		~StaticBody();

		void onConstruct() override;

		void addCollider(Collider* aCollider);

	private:
		physx::PxRigidStatic* mBody;
		std::vector<Collider*> mColliders;

		void _removeCollider(Collider* aCollider);
};

#endif // staticbody_h__