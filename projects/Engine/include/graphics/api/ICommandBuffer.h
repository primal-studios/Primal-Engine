#ifndef icommandbuffer_h__
#define icommandbuffer_h__

#include "graphics/ClearValue.h"
#include "graphics/api/ICommandPool.h"
#include "graphics/api/IFramebuffer.h"
#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IRenderPass.h"
#include "math/Vector4.h"

#include <cstdint>

struct CommandBufferCreateInfo
{
	ICommandPool* pool;
	bool primary;
};

enum ECommandBufferUsageFlagBits : uint32_t
{
	COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT		= 0x01,
	COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE	= 0x02,
	COMMAND_BUFFER_USAGE_SIMULATANEOUS_USE		= 0x04
};

using CommandBufferUsageFlags = uint32_t;

enum EQueryPipelineStatisticsFlagBits : uint32_t
{
	QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES						= 0x00000001,
	QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES						= 0x00000002,
	QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS						= 0x00000004,
	QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_INVOCATIONS					= 0x00000008,
	QUERY_PIPELINE_STATISTIC_GEOMETRY_SHADER_PRIMITIVES						= 0x00000010,
	QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS							= 0x00000020,
	QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES							= 0x00000040,
	QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS					= 0x00000080,
	QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES			= 0x00000100,
	QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS		= 0x00000200,
	QUERY_PIPELINE_STATISTIC_COMPUTE_SHADER_INVOCATIONS						= 0x00000400,
};

using QueryPipelineStatisticsFlags = uint32_t;

struct CommandBufferInheritanceInfo
{
	IRenderPass* renderPass;
	uint32_t subPass;
	IFramebuffer* frameBuffer;
	bool occlusionQueryEnable;
	bool queryPrecsise;
	QueryPipelineStatisticsFlags pipelineStatistics;
};

struct CommandBufferRecordInfo
{
	CommandBufferUsageFlags flags;
	CommandBufferInheritanceInfo* inheritance;
};

enum class ESubpassContents
{
	SUBPASS_CONTENTS_INLINE						= 0,
	SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS	= 1
};

struct RenderPassRecordInfo
{
	IRenderPass* renderPass;
	IFramebuffer* frameBuffer;
	Vector4i renderArea;
	std::vector<ClearValue> clearValues;
	ESubpassContents subpassContents;
};

class ICommandBuffer
{
	public:
		explicit ICommandBuffer(IGraphicsContext* aContext);
		ICommandBuffer(const ICommandBuffer&) = delete;
		ICommandBuffer(ICommandBuffer&&) noexcept = delete;
		virtual ~ICommandBuffer() = default;

		ICommandBuffer& operator=(const ICommandBuffer&) = delete;
		ICommandBuffer& operator=(ICommandBuffer&&) noexcept = delete;

		virtual void addDependency(ICommandBuffer*) = 0;
		virtual void removeDependency(ICommandBuffer*) = 0;

		virtual void construct(const CommandBufferCreateInfo&) = 0;
		virtual void reconstruct(const CommandBufferCreateInfo&) = 0;

		virtual void record(const CommandBufferRecordInfo&) = 0;
		virtual void end() = 0;

		virtual void recordRenderPass(const RenderPassRecordInfo&) = 0;
		virtual void endRenderPass() = 0;
};

#endif // icommandbuffer_h__
