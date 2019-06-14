#ifndef meshasset_h__
#define meshasset_h__

#include "assets/Asset.h"

class MeshAsset final : public Asset
{
	friend class AssetManager;
	public:
		explicit MeshAsset(const std::string& aPath);

	private:
		void _load() override;

		std::string mPath;
};

#endif // meshasset_h__