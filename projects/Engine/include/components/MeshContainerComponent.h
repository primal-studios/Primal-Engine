#ifndef meshcontainercomponent_h__
#define meshcontainercomponent_h__

#include "ecs/Component.h"
#include "graphics/Mesh.h"

class MeshContainerComponent final : public Component
{
	public:
		explicit MeshContainerComponent(Mesh* aMesh)
			: mMesh(aMesh) {}
		~MeshContainerComponent() = default;

		const Mesh* getMesh() const
		{
			return mMesh;
		}

	private:
		Mesh* mMesh;
};

#endif // meshcontainercomponent_h__