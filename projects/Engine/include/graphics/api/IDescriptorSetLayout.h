#ifndef idescriptorsetlayout_h__
#define idescriptorsetlayout_h__

#include <vector>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/ISampler.h"

#include "graphics/ShaderStageFlags.h"

enum EDescriptorSetLayoutCreateFlagBits : uint32_t
{

};

using DescriptorSetLayoutCreateFlags = uint32_t;

enum class EDescriptorType : uint32_t
{
	SAMPLER = 0,
	COMBINED_IMAGE_SAMPLER = 1,
	SAMPLED_IMAGE = 2,
	STORAGE_IMAGE = 3,
	UNIFORM_TEXEL_BUFFER = 4,
	STORAGE_TEXEL_BUFFER = 5,
	UNIFORM_BUFFER = 6,
	STORAGE_BUFFER = 7,
	UNIFORM_BUFFER_DYNAMIC = 8,
	STORAGE_BUFFER_DYNAMIC = 9,
	INPUT_ATTACHMENT = 10
};

struct DescriptorSetLayoutBinding
{
	uint32_t binding;
	EDescriptorType type;
	ShaderStageFlags shaderStageFlags;
	std::vector<ISampler*> immutableSamplers;
};

struct DescriptorSetLayoutCreateInfo
{
	DescriptorSetLayoutCreateFlags flags;
	std::vector<DescriptorSetLayoutBinding> layoutBindings;
};

class IDescriptorSetLayout
{
	public:
		explicit IDescriptorSetLayout(IGraphicsContext* aContext);
		IDescriptorSetLayout(const IDescriptorSetLayout&) = delete;
		IDescriptorSetLayout(IDescriptorSetLayout&&) noexcept = delete;
		virtual ~IDescriptorSetLayout() = default;

		IDescriptorSetLayout& operator=(const IDescriptorSetLayout&) = delete;
		IDescriptorSetLayout& operator=(IDescriptorSetLayout&&) noexcept = delete;

		virtual void construct(const DescriptorSetLayoutCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // idescriptorsetlayout_h__