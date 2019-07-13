#ifndef samplerasset_h__
#define samplerasset_h__

#include <string>

#include "assets/Asset.h"
#include "graphics/api/ISampler.h"

class SamplerAsset : public Asset
{
	friend class AssetManager;
public:
	explicit SamplerAsset(const std::string& aFile);
	~SamplerAsset();

	ISampler* getSampler() const;
private:
	ISampler* mSampler = nullptr;

	std::string mFileName;
	void _load() override;
};

#endif // samplerasset_h__