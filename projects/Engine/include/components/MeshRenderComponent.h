#ifndef meshrendercomponent_h__
#define meshrendercomponent_h__

#include "ecs/Component.h"

class MeshContainerComponent;
class MeshRenderComponent final : public Component
{
	public:
		MeshRenderComponent();
		~MeshRenderComponent();

		void onConstruct() override;

		void onRender() override;

	private:
		MeshContainerComponent* mContainer;
};

#endif // meshrendercomponent_h__