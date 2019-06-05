#ifndef iimageview_h__
#define iimageview_h__

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IImage.h"
#include "graphics/DataFormat.h"
#include "graphics/ImageAspect.h"

#include <cstdint>

struct ImageSubresourceRange
{
	ImageAspectFlags aspect;
	uint32_t baseMipLevel;
	uint32_t levelCount;
	uint32_t baseArrayLayer;
	uint32_t layerCount;
};

enum class EImageViewType : uint32_t
{
	IMAGE_VIEW_TYPE_1D			= 0,
	IMAGE_VIEW_TYPE_2D			= 1,
	IMAGE_VIEW_TYPE_3D			= 2,
	IMAGE_VIEW_TYPE_CUBE		= 3,
	IMAGE_VIEW_TYPE_1D_ARRAY	= 4,
	IMAGE_VIEW_TYPE_2D_ARRAY	= 5,
	IMAGE_VIEW_TYPE_CUBE_ARRAY	= 6,
};

struct ImageViewCreateInfo
{
	IImage* image;
	EDataFormat format;
	EImageViewType viewType;
	ImageSubresourceRange subresourceRange;
};

class IImageView
{
	public:
		explicit IImageView(IGraphicsContext* aContext);
		IImageView(const IImageView&) = delete;
		IImageView(IImageView&&) noexcept = delete;
		virtual ~IImageView() = default;

		IImageView& operator=(const IImageView&) = delete;
		IImageView& operator=(IImageView&&) noexcept = delete;

		virtual void construct(const ImageViewCreateInfo&) = 0;
};

#endif // iimageview_h__
