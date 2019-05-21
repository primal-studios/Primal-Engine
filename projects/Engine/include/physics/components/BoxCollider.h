#ifndef boxcollider_h__
#define boxcollider_h__

#include "physics/components/Collider.h"
#include "math/Vector3.h"

class BoxCollider final : public Collider
{
	public:
		BoxCollider(const Vector3f& aSize);
		~BoxCollider();
};

#endif // boxcollider_h__