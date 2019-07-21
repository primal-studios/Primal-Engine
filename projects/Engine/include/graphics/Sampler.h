#ifndef sampler_h__
#define sampler_h__

#include <cstdint>

enum class ESamplerParameter : uint32_t
{
	MAX_LOD = 0x813B,
	MIN_LOD = 0x813A,
	LOD_BIAS = 0x8501,
	COMPARE_MODE = 0x884C,
	COMPARE_FUNC = 0x884D,
	WRAP_S = 0x2802,
	WRAP_T = 0x2803,
	WRAP_R = 0x8072,
	MIN_FILTER = 0x2801,
	MAG_FILTER = 0x2800,
	BORDER_COLOR = 0x1004
};

enum class ESamplerValues : uint32_t
{
	LINEAR = 0x2601,
	NEAREST = 0x2600,
	NEAREST_MIPMAP_NEAREST = 0x2700,
	LINEAR_MIPMAP_NEAREST = 0x2701,
	NEAREST_MIPMAP_LINEAR = 0x2702,
	LINEAR_MIPMAP_LINEAR = 0x2703,
	CLAMP_TO_BORDER = 0x812D,
	CLAMP_TO_EDGE = 0x812F,
	REPEAT = 0x2901,
	MIRRORED_REPEAT = 0x8370,
	COMPARE_REF_TO_TEXTURE = 0x884E,
	NONE = 0,
	LEQUAL = 0x0203,
	GEQUAL = 0x0206,
	LESS = 0x0201,
	GREATER = 0x0204,
	EQUAL = 0x0202,
	NOTEQUAL = 0x0205,
	ALWAYS = 0x0207,
	NEVER = 0x0200
};

class Sampler
{
	public:
		Sampler();
		Sampler(const Sampler&) = delete;
		Sampler(Sampler&&) noexcept = delete;
		~Sampler();

		Sampler& operator=(const Sampler&) = delete;
		Sampler& operator=(Sampler&&) noexcept = delete;

		void setParameteri(ESamplerParameter aParam, const int32_t aValue) const;
		void setParameteriv(ESamplerParameter aParam, int32_t* aValue) const;

		void setParameterf(ESamplerParameter aParam, const float aValue) const;
		void setParameterfv(ESamplerParameter aParam, float* aValue) const;

		void bind(const uint32_t aUnit) const;

		[[nodiscard]] uint32_t getId() const;

	private:
		uint32_t mId = 0;
};

#endif // sampler_h__
