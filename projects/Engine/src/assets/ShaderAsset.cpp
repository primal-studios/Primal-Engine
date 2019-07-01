#include "assets/ShaderAsset.h"
#include "core/PrimalAssert.h"
#include "filesystem/FileSystem.h"
#include "graphics/GraphicsFactory.h"
#include "graphics/RenderPassManager.h"

#include <json/json.hpp>

ShaderAsset::ShaderAsset(const std::string& aPath)
	: mPipeline(nullptr)
{
	mGraphicsPipelineCreateInfo = {};
	mPath = aPath;
}

ShaderAsset::~ShaderAsset()
{
	delete mGraphicsPipelineCreateInfo.vertexState;
	delete mGraphicsPipelineCreateInfo.assemblyState;
	delete mGraphicsPipelineCreateInfo.viewportState;
	delete mGraphicsPipelineCreateInfo.rasterizationState;
	delete mGraphicsPipelineCreateInfo.multisampleState;
	delete mGraphicsPipelineCreateInfo.depthStencilState;
	delete mGraphicsPipelineCreateInfo.colorBlendState;
	delete mGraphicsPipelineCreateInfo.dynamicState;
	delete mGraphicsPipelineCreateInfo.layout;
}

void ShaderAsset::_load()
{
	const std::string fileContent = FileSystem::instance().loadToString(mPath);

	const auto jsonValue = nlohmann::json::parse(fileContent);

	const auto shaders = jsonValue["shaders"];
	PRIMAL_ASSERT(shaders.is_object(), "Could not load shaders.");

	const std::string vertPath = shaders.contains("vertexPath") ? shaders["vertexPath"] : "";
	const std::string fragPath = shaders.contains("fragmentPath") ? shaders["fragmentPath"] : "";
	const std::string geomPath = shaders.contains("geometryPath") ? shaders["geometryPath"] : "";
	const std::string tesPath = shaders.contains("tesPath") ? shaders["tesPath"] : "";
	const std::string tcsPath = shaders.contains("tcsPath") ? shaders["tcsPath"] : "";

	if (!vertPath.empty())
	{
		auto data = FileSystem::instance().getBytes(vertPath);

		ShaderStageCreateInfo info = {};
		ShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.code = data;

		info.flags = 0;
		info.module = GraphicsFactory::instance().createShaderModule();
		info.module->construct(moduleCreateInfo);
		info.stage = SHADER_STAGE_VERTEX;
		info.name = vertPath;

		IShaderStage* stage = GraphicsFactory::instance().createShaderStage();
		stage->construct(info);

		mGraphicsPipelineCreateInfo.stages.push_back(stage);
	}

	if (!fragPath.empty())
	{
		auto data = FileSystem::instance().getBytes(fragPath);

		ShaderStageCreateInfo info = {};
		ShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.code = data;

		info.flags = 0;
		info.module = GraphicsFactory::instance().createShaderModule();
		info.module->construct(moduleCreateInfo);
		info.stage = SHADER_STAGE_FRAGMENT;
		info.name = fragPath;

		IShaderStage* stage = GraphicsFactory::instance().createShaderStage();
		stage->construct(info);

		mGraphicsPipelineCreateInfo.stages.push_back(stage);
	}

	if (!geomPath.empty())
	{
		auto data = FileSystem::instance().getBytes(geomPath);

		ShaderStageCreateInfo info = {};
		ShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.code = data;

		info.flags = 0;
		info.module = GraphicsFactory::instance().createShaderModule();
		info.module->construct(moduleCreateInfo);
		info.stage = SHADER_STAGE_GEOMETRY;
		info.name = geomPath;

		IShaderStage* stage = GraphicsFactory::instance().createShaderStage();
		stage->construct(info);

		mGraphicsPipelineCreateInfo.stages.push_back(stage);
	}

	if (!tcsPath.empty())
	{
		auto data = FileSystem::instance().getBytes(tcsPath);

		ShaderStageCreateInfo info = {};
		ShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.code = data;

		info.flags = 0;
		info.module = GraphicsFactory::instance().createShaderModule();
		info.module->construct(moduleCreateInfo);
		info.stage = SHADER_STAGE_TESSELLATION_CONTROL;
		info.name = tcsPath;

		IShaderStage* stage = GraphicsFactory::instance().createShaderStage();
		stage->construct(info);

		mGraphicsPipelineCreateInfo.stages.push_back(stage);
	}

	if (!tesPath.empty())
	{
		auto data = FileSystem::instance().getBytes(tesPath);

		ShaderStageCreateInfo info = {};
		ShaderModuleCreateInfo moduleCreateInfo = {};
		moduleCreateInfo.flags = 0;
		moduleCreateInfo.code = data;

		info.flags = 0;
		info.module = GraphicsFactory::instance().createShaderModule();
		info.module->construct(moduleCreateInfo);
		info.stage = SHADER_STAGE_TESSELLATION_EVALUATION;
		info.name = tesPath;

		IShaderStage* stage = GraphicsFactory::instance().createShaderStage();
		stage->construct(info);

		mGraphicsPipelineCreateInfo.stages.push_back(stage);
	}

	PRIMAL_ASSERT(jsonValue.contains("pipelinelayout"), "Shader does not contain a pipeline layout.");
	const auto layout = jsonValue["pipelinelayout"];
	PRIMAL_ASSERT(layout.is_object(), "Could not load layout.");

	uint32_t flags = layout["flags"];
	const auto layouts = layout["layouts"];
	const auto pushconstants = layout["pushconstants"];

	PipelineLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.flags = flags;

	std::vector<IDescriptorSetLayout*> setLayouts;

	for (const auto & l : layouts)
	{
		IDescriptorSetLayout* setLayout = GraphicsFactory::instance().createDescriptorSetLayout();
		DescriptorSetLayoutCreateInfo setLayoutCreateInfo = {};

		uint32_t layoutFlags = l["flags"];
		const auto bindings = l["bindings"];

		setLayoutCreateInfo.flags = layoutFlags;

		for (const auto & b : bindings)
		{
			uint32_t binding = b["binding"];
			uint32_t descriptortype = b["descriptortype"];
			uint32_t descriptorcount = b["descriptorcount"];
			uint32_t stageflags = b["stageflags"];

			setLayoutCreateInfo.layoutBindings.push_back(DescriptorSetLayoutBinding{ binding, static_cast<EDescriptorType>(descriptortype), descriptorcount, stageflags, {} });
		}

		setLayout->construct(setLayoutCreateInfo);

		setLayouts.push_back(setLayout);
	}

	std::vector<PushConstantRange> pushConstantRanges;

	for (const auto & constant : pushconstants)
	{
		uint32_t stageflags = constant["stageflags"];
		uint32_t offset = constant["offset"];
		uint32_t size = constant["size"];

		pushConstantRanges.push_back(PushConstantRange{ stageflags, offset, size });
	}

	layoutCreateInfo.pushConstantRanges = pushConstantRanges;
	layoutCreateInfo.setLayouts = setLayouts;

	IPipelineLayout* pipelineLayout = GraphicsFactory::instance().createPipelineLayout();
	pipelineLayout->construct(layoutCreateInfo);

	mGraphicsPipelineCreateInfo.layout = pipelineLayout;

	PRIMAL_ASSERT(jsonValue.contains("graphicspipeline"), "Shader does not contain a graphics pipeline.");
	const auto pipeline = jsonValue["graphicspipeline"];
	PRIMAL_ASSERT(pipeline.is_object(), "Could not load pipeline.");

	const auto vertexState = pipeline["vertexstate"];
	PRIMAL_ASSERT(vertexState.is_object(), "Could not load vertex state.");

	mGraphicsPipelineCreateInfo.vertexState = new PipelineVertexStateCreateInfo;

	for (const auto& bindingdesc : vertexState["bindingdescriptions"])
	{
		uint32_t binding = bindingdesc["binding"];
		uint32_t stride = bindingdesc["stride"];
		uint32_t rate = bindingdesc["rate"];

		mGraphicsPipelineCreateInfo.vertexState->bindingDescriptions.push_back(VertexInputBindingDescription{ binding, stride, static_cast<EVertexInputRate>(rate) });
	}

	for (const auto& attributedesc : vertexState["attributedescriptions"])
	{
		uint32_t location = attributedesc["location"];
		uint32_t binding = attributedesc["binding"];
		uint32_t format = attributedesc["format"];
		uint32_t offset = attributedesc["offset"];

		mGraphicsPipelineCreateInfo.vertexState->attributeDescriptions.push_back(VertexInputAttributeDescription{ location, binding, static_cast<EDataFormat>(format), offset });
	}

	mGraphicsPipelineCreateInfo.vertexState->flags = 0;

	mGraphicsPipelineCreateInfo.assemblyState = new PipelineInputAssemblyStateCreateInfo;

	const auto assemblyState = pipeline["assemblystate"];
	uint32_t topology = assemblyState["topology"];
	bool primitiverestartenable = assemblyState["primitiverestartenable"];

	mGraphicsPipelineCreateInfo.assemblyState->topology = static_cast<PrimitiveTopology>(topology);
	mGraphicsPipelineCreateInfo.assemblyState->primitiveRestartEnable = primitiverestartenable;
	mGraphicsPipelineCreateInfo.assemblyState->flags = 0;

	mGraphicsPipelineCreateInfo.viewportState = new PipelineViewportStateCreateInfo;

	const auto viewportState = pipeline["viewportstate"];
	for (const auto& viewport : viewportState["viewports"])
	{
		float x = viewport["x"];
		float y = viewport["y"];
		float width = viewport["width"];
		float height = viewport["height"];
		float mindepth = viewport["mindepth"];
		float maxdepth = viewport["maxdepth"];

		mGraphicsPipelineCreateInfo.viewportState->viewports.push_back(Viewport{ x, y, width, height, mindepth, maxdepth });
	}

	for (const auto& rect : viewportState["rectangles"])
	{
		int32_t x = rect["x"];
		int32_t y = rect["y"];
		int32_t z = rect["z"];
		int32_t w = rect["w"];

		mGraphicsPipelineCreateInfo.viewportState->rectangles.emplace_back(Vector4i{ x, y, z, w });
	}

	mGraphicsPipelineCreateInfo.viewportState->flags = 0;

	const auto rasterizationState = pipeline["rasterizationstate"];

	mGraphicsPipelineCreateInfo.rasterizationState = new PipelineRasterizationStateCreateInfo;

	bool depthclampenable = rasterizationState["depthclampenable"];
	bool rasterizerdiscardenable = rasterizationState["rasterizerdiscardenable"];
	uint32_t polygonmode = rasterizationState["polygonmode"];
	uint32_t cullmode = rasterizationState["cullmode"];
	uint32_t frontface = rasterizationState["frontface"];
	bool depthbiasenable = rasterizationState["depthbiasenable"];
	float depthbiasconstantfactor = rasterizationState["depthbiasconstantfactor"];
	float depthbiasclamp = rasterizationState["depthbiasclamp"];
	float depthbiasslopefactor = rasterizationState["depthbiasslopefactor"];
	float linewidth = rasterizationState["linewidth"];

	mGraphicsPipelineCreateInfo.rasterizationState->depthClampEnable = depthclampenable;
	mGraphicsPipelineCreateInfo.rasterizationState->rasterizerDiscardEnable = rasterizerdiscardenable;
	mGraphicsPipelineCreateInfo.rasterizationState->polygonMode = static_cast<EPolygonMode>(polygonmode);
	mGraphicsPipelineCreateInfo.rasterizationState->cullMode = static_cast<ECullMode>(cullmode);
	mGraphicsPipelineCreateInfo.rasterizationState->frontFace = static_cast<EFrontFace>(frontface);
	mGraphicsPipelineCreateInfo.rasterizationState->depthBiasEnable = depthbiasenable;
	mGraphicsPipelineCreateInfo.rasterizationState->depthBiasConstantFactor = depthbiasconstantfactor;
	mGraphicsPipelineCreateInfo.rasterizationState->depthBiasClamp = depthbiasclamp;
	mGraphicsPipelineCreateInfo.rasterizationState->depthBiasSlopeFactor = depthbiasslopefactor;
	mGraphicsPipelineCreateInfo.rasterizationState->lineWidth = linewidth;
	mGraphicsPipelineCreateInfo.rasterizationState->flags = 0;

	mGraphicsPipelineCreateInfo.multisampleState = new PipelineMultisampleStateCreateInfo;

	const auto multisamplestate = pipeline["multisamplestate"];
	uint32_t samples = multisamplestate["rasterizationsamples"];
	bool sampleShading = multisamplestate["sampleshadingenable"];
	float minSampleShading = multisamplestate["minsampleshading"];
	uint32_t sampleMask = multisamplestate["samplemask"]; // currently unused
	bool alphaToCoverage = multisamplestate["alphatocoverageenable"];
	bool alphaToOne = multisamplestate["alphatooneenable"];

	mGraphicsPipelineCreateInfo.multisampleState->rasterizationSamples = samples;
	mGraphicsPipelineCreateInfo.multisampleState->sampleShadingEnable = sampleShading;
	mGraphicsPipelineCreateInfo.multisampleState->minSampleShading = minSampleShading;
	mGraphicsPipelineCreateInfo.multisampleState->sampleMask = nullptr;
	mGraphicsPipelineCreateInfo.multisampleState->alphaToCoverageEnable = alphaToCoverage;
	mGraphicsPipelineCreateInfo.multisampleState->alphaToOneEnable = alphaToOne;
	mGraphicsPipelineCreateInfo.multisampleState->flags = 0;

	mGraphicsPipelineCreateInfo.depthStencilState = new PipelineDepthStencilStateCreateInfo;

	const auto depthstencilstate = pipeline["depthstencilstate"];
	bool depthTest = depthstencilstate["depthtestenable"];
	bool depthWrite = depthstencilstate["depthwriteenable"];
	uint32_t depthCompareOp = depthstencilstate["depthcompareop"];
	bool depthBoundTest = depthstencilstate["depthboundstestenable"];
	bool stencilTest = depthstencilstate["depthtestenable"];
	uint32_t frontFail = depthstencilstate["front"]["failop"];
	uint32_t frontPass = depthstencilstate["front"]["passop"];
	uint32_t frontDepthFail = depthstencilstate["front"]["depthfailop"];
	uint32_t frontCompare = depthstencilstate["front"]["compareop"];
	uint32_t frontCompareMask = depthstencilstate["front"]["comparemask"];
	uint32_t frontWriteMask = depthstencilstate["front"]["writemask"];
	uint32_t frontReference = depthstencilstate["front"]["reference"];
	uint32_t backFail = depthstencilstate["back"]["failop"];
	uint32_t backPass = depthstencilstate["back"]["passop"];
	uint32_t backDepthFail = depthstencilstate["back"]["depthfailop"];
	uint32_t backCompare = depthstencilstate["back"]["compareop"];
	uint32_t backCompareMask = depthstencilstate["back"]["comparemask"];
	uint32_t backWriteMask = depthstencilstate["back"]["writemask"];
	uint32_t backReference = depthstencilstate["back"]["reference"];
	float minDepthBounds = depthstencilstate["mindepthbounds"];
	float maxDepthBounds = depthstencilstate["maxdepthbounds"];

	mGraphicsPipelineCreateInfo.depthStencilState->depthTestEnable = depthTest;
	mGraphicsPipelineCreateInfo.depthStencilState->depthWriteEnable = depthWrite;
	mGraphicsPipelineCreateInfo.depthStencilState->depthCompareOp = static_cast<ECompareOp>(depthCompareOp);
	mGraphicsPipelineCreateInfo.depthStencilState->depthBoundsTestEnable = depthBoundTest;
	mGraphicsPipelineCreateInfo.depthStencilState->stencilTestEnable = stencilTest;
	mGraphicsPipelineCreateInfo.depthStencilState->front = StencilOpState {
		static_cast<EStencilOp>(frontFail),
		static_cast<EStencilOp>(frontPass),
		static_cast<EStencilOp>(frontDepthFail),
		static_cast<ECompareOp>(frontCompare),
		frontCompareMask,
		frontWriteMask,
		frontReference
	};
	mGraphicsPipelineCreateInfo.depthStencilState->back = StencilOpState{
		static_cast<EStencilOp>(backFail),
		static_cast<EStencilOp>(backPass),
		static_cast<EStencilOp>(backDepthFail),
		static_cast<ECompareOp>(backCompare),
		backCompareMask,
		backWriteMask,
		backReference
	};
	mGraphicsPipelineCreateInfo.depthStencilState->minDepthBounds = minDepthBounds;
	mGraphicsPipelineCreateInfo.depthStencilState->maxDepthBounds = maxDepthBounds;
	mGraphicsPipelineCreateInfo.depthStencilState->flags = 0;

	mGraphicsPipelineCreateInfo.colorBlendState = new PipelineColorBlendStateCreateInfo;
	
	const auto colorblendstate = pipeline["colorblendstate"];
	bool logicOpEnable = colorblendstate["logicopenable"];
	uint32_t logicOp = colorblendstate["logicop"];

	mGraphicsPipelineCreateInfo.colorBlendState->logicOpEnable = logicOpEnable;
	mGraphicsPipelineCreateInfo.colorBlendState->logicOp = static_cast<ELogicOp>(logicOp);

	for (const auto& attachment : colorblendstate["attachments"])
	{
		bool blendEnable = attachment["blendenable"];
		uint32_t srcColorBlendFactor = attachment["srccolorblendfactor"];
		uint32_t dstColorBlendFactor = attachment["dstcolorblendfactor"];
		uint32_t colorBlendOp = attachment["colorblendop"];
		uint32_t srcAlphaBlendFactor = attachment["srcalphablendfactor"];
		uint32_t dstAlphaBlendFactor = attachment["dstalphablendfactor"];
		uint32_t alphaBlendOp = attachment["alphablendop"];
		uint32_t colorWriteMask = attachment["colorwritemask"];

		mGraphicsPipelineCreateInfo.colorBlendState->attachments.emplace_back(PipelineColorBlendAttachmentState{ blendEnable, static_cast<EBlendFactor>(srcColorBlendFactor),
			static_cast<EBlendFactor>(dstColorBlendFactor), static_cast<EBlendOp>(colorBlendOp), static_cast<EBlendFactor>(srcAlphaBlendFactor),
			static_cast<EBlendFactor>(dstAlphaBlendFactor), static_cast<EBlendOp>(alphaBlendOp), colorWriteMask });
	}

	size_t blendConstantIt = 0;
	for (float constant : colorblendstate["blendconstants"])
	{
		mGraphicsPipelineCreateInfo.colorBlendState->blendConstants[blendConstantIt++] = constant;
	}

	mGraphicsPipelineCreateInfo.colorBlendState->flags = 0;

	const auto dynamicstate = pipeline["dynamicstate"];

	std::vector<EDynamicState> states;
	for (uint32_t state : dynamicstate["states"])
	{
		states.push_back(static_cast<EDynamicState>(state));
	}

	mGraphicsPipelineCreateInfo.dynamicState = new PipelineDynamicStateCreateInfo;
	mGraphicsPipelineCreateInfo.dynamicState->dynamicStates = states;
	mGraphicsPipelineCreateInfo.dynamicState->flags = 0;

	mGraphicsPipelineCreateInfo.flags = 0;

	mGraphicsPipelineCreateInfo.renderPass = RenderPassManager::instance().findPass(jsonValue["renderpass"]);
	mGraphicsPipelineCreateInfo.subPass = jsonValue["subpass"];

	mPipeline = GraphicsFactory::instance().createGraphicsPipeline();
	mPipeline->construct(mGraphicsPipelineCreateInfo);

	int jonathan = 0;
}