#ifndef meshasset_h__
#define meshasset_h__

#include "assets/Asset.h"

class MeshAsset : public Asset
{
	public:
		MeshAsset(const std::string& aPath);
};

#endif // meshasset_h__