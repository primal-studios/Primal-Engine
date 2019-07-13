#ifndef itexture_h__
#define itexture_h__

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IImage.h"
#include "graphics/api/IImageView.h"
#include "graphics/api/ISampler.h"
#include "graphics/api/IDescriptorPool.h"

class TextureAsset;
struct TextureCreateInfo
{
	TextureAsset* textureAsset;
	ISampler* sampler;
};

class ITexture
{
	public:
		explicit ITexture(IGraphicsContext* aContext);
		ITexture(const ITexture&) = delete;
		ITexture(ITexture&&) noexcept = delete;
		virtual ~ITexture() = default;

		ITexture& operator=(const ITexture&) = delete;
		ITexture& operator=(ITexture&&) noexcept = delete;

		virtual void construct(const TextureCreateInfo&) = 0;

		virtual void bind(const uint32_t aBinding = 0) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // itexture_h__
