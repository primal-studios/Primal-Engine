#ifndef meshrendercomponent_h__
#define meshrendercomponent_h__

#include "ecs/Component.h"

class MeshRenderComponent final : public Component
{
	public:
		MeshRenderComponent();
		~MeshRenderComponent();

		void onRender() override;
};

#endif // meshrendercomponent_h__