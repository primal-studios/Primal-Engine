#include "components/MeshRenderComponent.h"
#include "ecs/Entity.h"

#include "components/MeshContainerComponent.h"
#include "graphics/VertexArray.h"

#include "core/PrimalAssert.h"

MeshRenderComponent::MeshRenderComponent()
{
	mContainer = nullptr;
	mVAO = nullptr;
}

MeshRenderComponent::~MeshRenderComponent()
{
	
}

void MeshRenderComponent::onConstruct()
{
	mContainer = entity->getComponent<MeshContainerComponent>();
	PRIMAL_INTERNAL_ASSERT(mContainer != nullptr, "MeshRenderComponent needs a MeshContainerComponent to be on the same entity");

	mVAO = mContainer->getMesh()->getVAO();
}

void MeshRenderComponent::onRender()
{
	mVAO->bind();

	glDrawElements(GL_TRIANGLES, mVAO->getIndexBuffer()->count(), GL_UNSIGNED_INT, nullptr);

	mVAO->unbind();
}
