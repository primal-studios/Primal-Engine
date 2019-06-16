#ifndef meshasset_h__
#define meshasset_h__

#include "assets/Asset.h"
#include "graphics/Mesh.h"

class MeshAsset final : public Asset
{
	friend class AssetManager;
	public:
		explicit MeshAsset(const std::string& aPath);
		~MeshAsset();

		Mesh* getMesh(const size_t aIndex = 0);

	private:
		void _load() override;

		std::vector<Mesh*> mMeshes;

		std::string mPath;
};

#endif // meshasset_h__