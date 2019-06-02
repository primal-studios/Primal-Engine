#ifndef igraphicspipeline_h__
#define igraphicspipeline_h__

#include <vector>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IShaderStage.h"
#include "graphics/api/IRenderPass.h"
#include "graphics/api/IPipelineLayout.h"

#include "graphics/CompareOp.h"
#include "graphics/DataFormat.h"
#include "math/Vector4.h"

class IGraphicsPipeline;

enum EPipelineCreateFlagBits : uint32_t
{
	PIPELINE_CREATE_DISABLE_OPTIMIZATION = 0x00000001,
	PIPELINE_CREATE_ALLOW_DERIVATIVES = 0x00000002,
	PIPELINE_CREATE_DERIVATIVE = 0x00000004,
	PIPELINE_CREATE_VIEW_INDEX_FROM_DEVICE_INDEX = 0x00000008,
	PIPELINE_CREATE_DISPATCH_BASE = 0x00000010
};

using PipelineCreateFlags = uint32_t;

enum EPipelineVertexInputStateCreateFlagBits : uint32_t
{
	
};

using PipelineVertexInputStateCreateFlags = uint32_t;

enum class EVertexInputRate : uint32_t
{
	VERTEX_INPUT_RATE_VERTEX = 0,
	VERTEX_INPUT_RATE_INSTANCE = 1
};

enum EInputAssemblyStateCreateFlagBits : uint32_t
{
	
};

using InputAssemblyStateCreateFlags = uint32_t;

enum class PrimitiveTopology : uint32_t
{
	PRIMITIVE_TOPOLOGY_POINT_LIST = 0,
	PRIMITIVE_TOPOLOGY_LINE_LIST = 1,
	PRIMITIVE_TOPOLOGY_LINE_STRIP = 2,
	PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = 3,
	PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 4,
	PRIMITIVE_TOPOLOGY_TRIANGLE_FAN = 5,
	PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY = 6,
	PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY = 7,
	PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY = 8,
	PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY = 9,
	PRIMITIVE_TOPOLOGY_PATCH_LIST = 10,
};

enum EPipelineTesselationStateCreateFlagBits : uint32_t
{
	
};

using PipelineTesselationStateCreateFlags = uint32_t;

enum EPipelineViewportStateCreateFlagBits : uint32_t
{
	
};

using PipelineViewportStateCreateFlags = uint32_t;

enum EPipelineRasterizationStateCreateFlagBits : uint32_t
{
	
};

using PipelineRasterizationStateCreateFlags = uint32_t;

enum class EPolygonMode : uint32_t
{
	FILL = 0,
	LINE = 1,
	POINT = 2
};

enum class ECullMode : uint32_t
{
	NONE = 0,
	FRONT = 0x00000001,
	BACK = 0x00000002,
	FRONT_AND_BACK = 0x00000003,
};

enum class EFrontFace : uint32_t
{
	COUNTER_CLOCKWISE = 0,
	CLOCKWISE = 1
};

enum EPipelineMultisampleStateCreateFlagBits : uint32_t
{
	
};

using PipelineMultisampleStateCreateFlags = uint32_t;

enum ESampleCountFlagBits : uint32_t
{
	SAMPLE_COUNT_1 = 0x00000001,
	SAMPLE_COUNT_2 = 0x00000002,
	SAMPLE_COUNT_4 = 0x00000004,
	SAMPLE_COUNT_8 = 0x00000008,
	SAMPLE_COUNT_16 = 0x00000010,
	SAMPLE_COUNT_32 = 0x00000020,
	SAMPLE_COUNT_64 = 0x00000040,
};

using SampleCountFlags = uint32_t;

enum EPipelineDepthStencilStateCreateFlags : uint32_t
{
	
};

using PipelineDepthStencilStateCreateFlags = uint32_t;

enum class EStencilOp : uint32_t
{
	STENCIL_OP_KEEP = 0,
	STENCIL_OP_ZERO = 1,
	STENCIL_OP_REPLACE = 2,
	STENCIL_OP_INCREMENT_AND_CLAMP = 3,
	STENCIL_OP_DECREMENT_AND_CLAMP = 4,
	STENCIL_OP_INVERT = 5,
	STENCIL_OP_INCREMENT_AND_WRAP = 6,
	STENCIL_OP_DECREMENT_AND_WRAP = 7,
};

enum EPipelineColorBlendStateCreateFlagBits : uint32_t
{
	
};

using PipelineColorBlendStateCreateFlags = uint32_t;

enum class ELogicOp : uint32_t
{
	LOGIC_OP_CLEAR = 0,
	LOGIC_OP_AND = 1,
	LOGIC_OP_AND_REVERSE = 2,
	LOGIC_OP_COPY = 3,
	LOGIC_OP_AND_INVERTED = 4,
	LOGIC_OP_NO_OP = 5,
	LOGIC_OP_XOR = 6,
	LOGIC_OP_OR = 7,
	LOGIC_OP_NOR = 8,
	LOGIC_OP_EQUIVALENT = 9,
	LOGIC_OP_INVERT = 10,
	LOGIC_OP_OR_REVERSE = 11,
	LOGIC_OP_COPY_INVERTED = 12,
	LOGIC_OP_OR_INVERTED = 13,
	LOGIC_OP_NAND = 14,
	LOGIC_OP_SET = 15,
};

enum class EBlendFactor : uint32_t
{
	ZERO = 0,
	ONE = 1,
	SRC_COLOR = 2,
	ONE_MINUS_SRC_COLOR = 3,
	DST_COLOR = 4,
	ONE_MINUS_DST_COLOR = 5,
	SRC_ALPHA = 6,
	ONE_MINUS_SRC_ALPHA = 7,
	DST_ALPHA = 8,
	ONE_MINUS_DST_ALPHA = 9,
	CONSTANT_COLOR = 10,
	ONE_MINUS_CONSTANT_COLOR = 11,
	CONSTANT_ALPHA = 12,
	ONE_MINUS_CONSTANT_ALPHA = 13,
	SRC_ALPHA_SATURATE = 14,
	SRC1_COLOR = 15,
	ONE_MINUS_SRC1_COLOR = 16,
	SRC1_ALPHA = 17,
	ONE_MINUS_SRC1_ALPHA = 18,
};

enum class EBlendOp : uint32_t
{
	ADD = 0,
	SUBTRACT = 1,
	REVERSE_SUBTRACT = 2,
	MIN = 3,
	MAX = 4
};

enum EPipelineDynamicStateCreateFlagBits : uint32_t
{
	
};

using PipelineDynamicStateCreateFlags = uint32_t;

enum class EDynamicState : uint32_t
{
	VIEWPORT = 0,
	SCISSOR = 1,
	LINE_WIDTH = 2,
	DEPTH_BIAS = 3,
	BLEND_CONSTANTS = 4,
	DEPTH_BOUNDS = 5,
	STENCIL_COMPARE_MASK = 6,
	STENCIL_WRITE_MASK = 7,
	STENCIL_REFERENCE = 8
};

enum EPipelineLayoutCreateFlagBits : uint32_t
{
	
};

using PipelineLayoutCreateFlags = uint32_t;

struct VertexInputAttributeDescription
{
	uint32_t location;
	uint32_t binding;
	EDataFormat format;
	uint32_t offset;
};

struct VertexInputBindingDescription
{
	uint32_t binding;
	uint32_t stride;
	EVertexInputRate rate;
};

struct PipelineVertexStateCreateInfo
{
	PipelineVertexInputStateCreateFlags flags;
	std::vector<VertexInputBindingDescription> bindingDescriptions;
	std::vector<VertexInputAttributeDescription> attributeDescriptions;
};

struct PipelineInputAssemblyStateCreateInfo
{
	InputAssemblyStateCreateFlags flags;
	PrimitiveTopology topology;
	bool primitiveRestartEnable;
};

struct PipelineTesselationStateCreateInfo
{
	PipelineTesselationStateCreateFlags flags;
	uint32_t patchControlPoints;
};

struct Viewport
{
	float x, y, width, height, minDepth, maxDepth;
};

struct PipelineViewportStateCreateInfo
{
	PipelineViewportStateCreateFlags flags;
	std::vector<Viewport> viewports;
	std::vector<Vector4i> rectangles;
};

struct PipelineRasterizationStateCreateInfo
{
	PipelineRasterizationStateCreateFlags flags;
	bool depthClampEnable;
	bool rasterizerDiscardEnable;
	EPolygonMode polygonMode;
	ECullMode cullMode;
	EFrontFace frontFace;
	bool depthBiasEnable;
	float depthBiasConstantFactor;
	float depthBiasClamp;
	float depthBiasSlopeFactor;
	float lineWidth;
};

struct PipelineMultisampleStateCreateInfo
{
	PipelineMultisampleStateCreateFlags flags;
	SampleCountFlags rasterizationSamples;
	bool sampleShadingEnable;
	float minSampleShading;
	const uint32_t* sampleMask;
	bool alphaToCoverageEnable;
	bool alphaToOneEnable;
};

struct StencilOpState
{
	EStencilOp failOp;
	EStencilOp passOp;
	EStencilOp depthFailOp;
	ECompareOp compareOp;
	uint32_t compareMask;
	uint32_t writeMask;
	uint32_t reference;
};

struct PipelineDepthStencilStateCreateInfo
{
	PipelineDepthStencilStateCreateFlags flags;
	bool depthTestEnable;
	bool depthWriteEnable;
	ECompareOp depthCompareOp;
	bool depthBoundsTestEnable;
	bool stencilTestEnable;
	StencilOpState front;
	StencilOpState back;
	float minDepthBounds;
	float maxDepthBounds;
};

struct PipelineColorBlendAttachmentState
{
	bool blendEnable;
	EBlendFactor srcColorBlendFactor;
	EBlendFactor dstColorBlendFactor;
	EBlendOp colorBlendOp;
	EBlendFactor srcAlphaBlendFactor;
	EBlendFactor dstAlphaBlendFactor;
	EBlendOp alphaBlendOp;
	uint32_t colorWriteMask;
};

struct PipelineColorBlendStateCreateInfo
{
	PipelineColorBlendStateCreateFlags flags;
	bool logicOpEnable;
	ELogicOp logicOp;
	std::vector<PipelineColorBlendAttachmentState> attachments;
	float blendConstants[4];
};

struct PipelineDynamicStateCreateInfo
{
	PipelineDynamicStateCreateFlags flags;
	std::vector<EDynamicState> dynamicStates;
};

struct GraphicsPipelineCreateInfo
{
	PipelineCreateFlags flags;
	std::vector<IShaderStage*> stages;
	PipelineVertexStateCreateInfo* vertexState;
	PipelineInputAssemblyStateCreateInfo* assemblyState;
	PipelineTesselationStateCreateInfo* tesselationState;
	PipelineViewportStateCreateInfo* viewportState;
	PipelineRasterizationStateCreateInfo* rasterizationState;
	PipelineMultisampleStateCreateInfo* multisampleState;
	PipelineDepthStencilStateCreateInfo* depthStencilState;
	PipelineColorBlendStateCreateInfo* colorBlendState;
	PipelineDynamicStateCreateInfo* dynamicState;
	IPipelineLayout* layout;
	IRenderPass* renderPass;
	uint32_t subPass;
	IGraphicsPipeline* basePipelineHandle;
	int32_t basePipelineIndex;
};

class IGraphicsPipeline
{
	public:
		explicit IGraphicsPipeline(IGraphicsContext* aContext);
		IGraphicsPipeline(const IGraphicsPipeline&) = delete;
		IGraphicsPipeline(IGraphicsPipeline&&) noexcept = delete;
		virtual ~IGraphicsPipeline() = default;

		IGraphicsPipeline& operator=(const IGraphicsPipeline&) = delete;
		IGraphicsPipeline& operator=(IGraphicsPipeline&&) noexcept = delete;

		virtual void construct(const GraphicsPipelineCreateInfo& aInfo) = 0;
		virtual void reconstruct(const GraphicsPipelineCreateInfo& aInfo) = 0;
	protected:
		IGraphicsContext* mContext;
};

#endif // igraphicspipeline_h__