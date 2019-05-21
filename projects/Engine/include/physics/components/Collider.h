#ifndef collider_h__
#define collider_h__

#include <physx/PxShape.h>

#include "ecs/Component.h"

class Collider : public Component
{
	public:
		Collider();
		~Collider();

	protected:
		physx::PxShape* mShape;
		bool mIsTrigger;
};

#endif // collider_h__