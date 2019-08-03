#include "components/MeshRenderComponent.h"
#include "ecs/Entity.h"

#include "components/MeshContainerComponent.h"

#include "core/PrimalAssert.h"

MeshRenderComponent::MeshRenderComponent()
{
	mContainer = nullptr;
}

MeshRenderComponent::~MeshRenderComponent()
{
	
}

void MeshRenderComponent::onConstruct()
{
	mContainer = entity->getComponent<MeshContainerComponent>();
	PRIMAL_INTERNAL_ASSERT(mContainer != nullptr, "MeshRenderComponent needs a MeshContainerComponent to be on the same entity");
}

void MeshRenderComponent::onRender()
{

}
