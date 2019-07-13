#include "graphics/api/ITexture.h"
#include "assets/TextureAsset.h"

ITexture::ITexture(IGraphicsContext* aContext)
{
	mContext = aContext;
}
