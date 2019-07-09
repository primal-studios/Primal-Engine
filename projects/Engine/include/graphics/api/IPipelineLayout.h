#ifndef ipipelinelayout_h__
#define ipipelinelayout_h__

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IDescriptorSetLayout.h"

enum PipelineLayoutCreateFlagBits : uint32_t
{
	
};

using PipelineLayoutCreateFlags = uint32_t;

struct PushConstantRange
{
	ShaderStageFlags stageFlags;
	uint32_t offset;
	uint32_t size;
};

struct PipelineLayoutCreateInfo
{
	PipelineLayoutCreateFlags flags;
	std::vector<IDescriptorSetLayout*> setLayouts;
	std::vector<PushConstantRange> pushConstantRanges;
};

class IPipelineLayout
{
	public:
		explicit IPipelineLayout(IGraphicsContext* aContext);
		IPipelineLayout(const IPipelineLayout&) = delete;
		IPipelineLayout(IPipelineLayout&&) noexcept = delete;
		virtual ~IPipelineLayout() = default;

		IPipelineLayout& operator=(const IPipelineLayout&) = delete;
		IPipelineLayout& operator=(IPipelineLayout&&) noexcept = delete;

		virtual void construct(const PipelineLayoutCreateInfo& aInfo) = 0;
		virtual void reconstruct(const PipelineLayoutCreateInfo& aInfo) = 0;
		virtual void destroy() = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // ipipelinelayout_h__