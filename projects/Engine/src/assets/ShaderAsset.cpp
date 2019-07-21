#include "assets/AssetManager.h"
#include "assets/ShaderAsset.h"
#include "core/PrimalAssert.h"
#include "filesystem/FileSystem.h"

#include <json/json.hpp>

ShaderAsset::ShaderAsset(const std::string& aPath)
{
	mPath = aPath;
}

ShaderAsset::~ShaderAsset()
{
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

	}

	if (!fragPath.empty())
	{
		auto data = FileSystem::instance().getBytes(fragPath);

	}

	if (!geomPath.empty())
	{
		auto data = FileSystem::instance().getBytes(geomPath);

	}

	if (!tcsPath.empty())
	{
		auto data = FileSystem::instance().getBytes(tcsPath);

	}

	if (!tesPath.empty())
	{
		auto data = FileSystem::instance().getBytes(tesPath);

	}

	PRIMAL_ASSERT(jsonValue.contains("pipelinelayout"), "Shader does not contain a pipeline layout.");
	const auto layout = jsonValue["pipelinelayout"];
	PRIMAL_ASSERT(layout.is_object(), "Could not load layout.");

	uint32_t flags = layout["flags"];
	const auto layouts = layout["layouts"];
	const auto pushconstants = layout["pushconstants"];


	for (const auto & l : layouts)
	{
		uint32_t layoutFlags = l["flags"];
		const auto bindings = l["bindings"];

		for (const auto & b : bindings)
		{
			uint32_t binding = b["binding"];
			uint32_t descriptortype = b["descriptortype"];
			uint32_t descriptorcount = b["descriptorcount"];
			uint32_t stageflags = b["stageflags"];
		}

	}

	for (const auto & constant : pushconstants)
	{
		uint32_t stageflags = constant["stageflags"];
		uint32_t offset = constant["offset"];
		uint32_t size = constant["size"];

	}

	PRIMAL_ASSERT(jsonValue.contains("graphicspipeline"), "Shader does not contain a graphics pipeline.");
	const auto pipeline = jsonValue["graphicspipeline"];
	PRIMAL_ASSERT(pipeline.is_object(), "Could not load pipeline.");

	const auto vertexState = pipeline["vertexstate"];
	PRIMAL_ASSERT(vertexState.is_object(), "Could not load vertex state.");

	for (const auto& bindingdesc : vertexState["bindingdescriptions"])
	{
		uint32_t binding = bindingdesc["binding"];
		uint32_t stride = bindingdesc["stride"];
		uint32_t rate = bindingdesc["rate"];
	}

	for (const auto& attributedesc : vertexState["attributedescriptions"])
	{
		uint32_t location = attributedesc["location"];
		uint32_t binding = attributedesc["binding"];
		uint32_t format = attributedesc["format"];
		uint32_t offset = attributedesc["offset"];
	}

	const auto assemblyState = pipeline["assemblystate"];
	uint32_t topology = assemblyState["topology"];
	bool primitiverestartenable = assemblyState["primitiverestartenable"];

	const auto viewportState = pipeline["viewportstate"];
	for (const auto& viewport : viewportState["viewports"])
	{
		float x = viewport["x"];
		float y = viewport["y"];
		float width = viewport["width"];
		float height = viewport["height"];
		float mindepth = viewport["mindepth"];
		float maxdepth = viewport["maxdepth"];
	}

	for (const auto& rect : viewportState["rectangles"])
	{
		int32_t x = rect["x"];
		int32_t y = rect["y"];
		int32_t z = rect["z"];
		int32_t w = rect["w"];
	}

	const auto rasterizationState = pipeline["rasterizationstate"];

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

	const auto multisamplestate = pipeline["multisamplestate"];
	uint32_t samples = multisamplestate["rasterizationsamples"];
	bool sampleShading = multisamplestate["sampleshadingenable"];
	float minSampleShading = multisamplestate["minsampleshading"];
	uint32_t sampleMask = multisamplestate["samplemask"]; // currently unused
	bool alphaToCoverage = multisamplestate["alphatocoverageenable"];
	bool alphaToOne = multisamplestate["alphatooneenable"];

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
	
	const auto colorblendstate = pipeline["colorblendstate"];
	bool logicOpEnable = colorblendstate["logicopenable"];
	uint32_t logicOp = colorblendstate["logicop"];

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

	}

	size_t blendConstantIt = 0;
	for (float constant : colorblendstate["blendconstants"])
	{
	}

	const auto dynamicstate = pipeline["dynamicstate"];
	for (uint32_t state : dynamicstate["states"])
	{
	}
}
