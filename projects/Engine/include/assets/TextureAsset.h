#ifndef textureasset_h__
#define textureasset_h__

#include <vector>

#include "assets/Asset.h"
#include "graphics/api/ISampler.h"
#include "graphics/api/ITexture.h"

struct ImageFile
{
	std::vector<unsigned char> payload;
	uint32_t width;
	uint32_t height;
	uint32_t channels;
	uint32_t bitsPerPixel;
};

class TextureAsset final : public Asset
{
	friend class AssetManager;
	public:
		explicit TextureAsset(const std::string& aPath, const uint32_t aDesiredChannels);
		~TextureAsset();

		ImageFile getData() const;

		ITexture* getTexture() const;
		ISampler* getSampler() const;

	private:
		void _load() override;

		std::string mPath;
		uint32_t mDesiredChannels;

		ImageFile mFile;

		ITexture* mTexture;
		ISampler* mSampler;
};

#endif // textureasset_h__