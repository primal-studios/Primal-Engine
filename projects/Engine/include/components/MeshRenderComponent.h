#ifndef meshrendercomponent_h__
#define meshrendercomponent_h__

#include "ecs/Component.h"

#include "graphics/Material.h"

class MeshContainerComponent;
class MeshRenderComponent final : public Component
{
		friend class RenderSystem;
	public:
		MeshRenderComponent(MaterialInstance* aMaterialInstance);
		~MeshRenderComponent();

		void onConstruct() override;

		void onRender() override;

	private:
		MeshContainerComponent* mContainer;
		MaterialInstance* mMaterialInstance;
};

#endif // meshrendercomponent_h__