#ifndef isampler_h__
#define isampler_h__

#include "graphics/api/IGraphicsContext.h"
#include "graphics/CompareOp.h"

enum ESamplerCreateFlagBits : uint32_t
{
	
};

using SamplerCreateFlags = uint32_t;

enum class EFilter : uint32_t
{
	NEAREST = 0,
	LINEAR = 1,
	CUBIC_IMG = 1000015000
};

enum class ESamplerMipmapMode : uint32_t
{
	NEAREST = 0,
	LINEAR = 1
};

enum class ESamplerAddressMode : uint32_t
{
	REPEAT = 0,
	MIRRORED_REPEAT = 1,
	CLAMP_TO_EDGE = 2,
	CLAMP_TO_BORDER = 3,
	MIRROR_CLAMP_TO_EDGE = 4,
};

enum class EBorderColor : uint32_t
{
	FLOAT_TRANSPARENT_BLACK = 0,
	INT_TRANSPARENT_BLACK = 1,
	FLOAT_OPAQUE_BLACK = 2,
	INT_OPAQUE_BLACK = 3,
	FLOAT_OPAQUE_WHITE = 4,
	INT_OPAQUE_WHITE = 5,
};

struct SamplerCreateInfo
{
	SamplerCreateFlags flags;
	EFilter magFilter;
	EFilter minFilter;
	ESamplerMipmapMode mipmapMode;
	ESamplerAddressMode addressModeU;
	ESamplerAddressMode addressModeV;
	ESamplerAddressMode addressModeW;
	float mipLodBias;
	bool anisotropyEnable;
	float maxAnisotropy;
	bool compareEnable;
	ECompareOp compareOp;
	float minLod;
	float maxLod;
	EBorderColor borderColor;
	bool unnormalizedCoordinates;
};

class ISampler
{
	public:
		explicit ISampler(IGraphicsContext* aContext);
		ISampler(const ISampler&) = delete;
		ISampler(ISampler&&) noexcept = delete;
		virtual ~ISampler() = default;

		ISampler& operator=(const ISampler&) = delete;
		ISampler& operator=(ISampler&&) noexcept = delete;

		virtual void construct(const SamplerCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // isampler_h__