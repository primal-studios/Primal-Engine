#ifndef iimage_h__
#define iimage_h__

#include <graphics/api/IGraphicsContext.h>
#include <graphics/DataFormat.h>

#include <vector>

using ImageCreateFlags = uint32_t;

enum EImageCreateFlagBits : uint32_t
{
	IMAGE_CREATE_SPARSE_BINDING					= 0x00000001,
	IMAGE_CREATE_SPARSE_RESIDENCY				= 0x00000002,
	IMAGE_CREATE_SPARSE_ALIASED					= 0x00000004,
	IMAGE_CREATE_MUTABLE_FORMAT					= 0x00000008,
	IMAGE_CREATE_CUBE_COMPATIBLE				= 0x00000010,
	IMAGE_CREATE_2D_ARRAY_COMPATIBLE			= 0x00000020,
	IMAGE_CREATE_SPLIT_INSTANCE_BIND_REGIONS	= 0x00000040,
	IMAGE_CREATE_BLOCK_TEXEL_VIEW_COMPATIBLE	= 0x00000080,
	IMAGE_CREATE_EXTENDED_USAGE					= 0x00000100,
	IMAGE_CREATE_DISJOINT						= 0x00000200,
	IMAGE_CREATE_ALIAS							= 0x00000400,
	IMAGE_CREATE_PROTECTED						= 0x00000800,
};

enum class EImageDimension : uint32_t
{
	IMAGE_1D = 0,
	IMAGE_2D = 1,
	IMAGE_3D = 2
};

using ImageSampleFlags = uint32_t;

enum EImageSampleFlagBits : uint32_t
{
	IMAGE_SAMPLE_1	= 0x001,
	IMAGE_SAMPLE_2	= 0x002,
	IMAGE_SAMPLE_4	= 0x004,
	IMAGE_SAMPLE_8	= 0x008,
	IMAGE_SAMPLE_16 = 0x010,
	IMAGE_SAMPLE_32 = 0x020,
	IMAGE_SAMPLE_64 = 0x040
};

enum class EImageTiling : uint32_t
{
	IMAGE_TILING_OPTIMAL = 0,
	IMAGE_TILING_LINEAR  = 1
};

using ImageUsageFlags = uint32_t;

enum EImageUsageFlagBits : uint32_t
{
	IMAGE_USAGE_TRANSFER_SRC_BIT				= 0x00000001,
	IMAGE_USAGE_TRANSFER_DST_BIT				= 0x00000002,
	IMAGE_USAGE_SAMPLED_BIT						= 0x00000004,
	IMAGE_USAGE_STORAGE_BIT						= 0x00000008,
	IMAGE_USAGE_COLOR_ATTACHMENT_BIT			= 0x00000010,
	IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT	= 0x00000020,
	IMAGE_USAGE_TRANSIENT_ATTACHMENT			= 0x00000040,
	IMAGE_USAGE_INPUT_ATTACHMENT				= 0x00000080,
};

enum EImageLayout : uint32_t
{
	IMAGE_LAYOUT_UNDEFINED										= 0,
	IMAGE_LAYOUT_GENERAL										= 1,
	IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL						= 2,
	IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL				= 3,
	IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL				= 4,
	IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL						= 5,
	IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL							= 6,
	IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL							= 7,
	IMAGE_LAYOUT_PREINITIALIZED									= 8,
	IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL		= 1000117000,
	IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL		= 1000117001,
	IMAGE_LAYOUT_PRESENT_SRC_KHR								= 1000001002,
	IMAGE_LAYOUT_SHARED_PRESENT_KHR								= 1000111000,
};

struct ImageCreateInfo
{
	ImageCreateFlags flags;
	EImageDimension dimension;
	EDataFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t mipLayerCount;
	uint32_t arrayLayerCount;
	EImageSampleFlagBits samples;
	EImageTiling tiling;
	ImageUsageFlags usage;
	EImageLayout layout;
	std::vector<IGraphicsContext*> contexts;
};

class IImage
{
	public:
		explicit IImage(IGraphicsContext* aContext);
		IImage(const IImage&) = delete;
		IImage(IImage&&) noexcept = delete;
		virtual ~IImage() = default;
		
		IImage& operator=(const IImage&) = delete;
		IImage& operator=(IImage&&) noexcept = delete;

		virtual void construct(const ImageCreateInfo&) = 0;
};

#endif // iimage_h__
