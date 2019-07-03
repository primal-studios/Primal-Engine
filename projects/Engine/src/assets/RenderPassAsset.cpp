#include <utility>

#include <json/json.hpp>

#include "assets/RenderPassAsset.h"
#include "filesystem/FileSystem.h"
#include "graphics/GraphicsFactory.h"

RenderPassAsset::RenderPassAsset(std::string aPath)
	: mPath(std::move(aPath))
{
}

RenderPassAsset::~RenderPassAsset()
{
	// TODO: delete owned render pass?
	delete mRenderPass;
}

IRenderPass* RenderPassAsset::getRenderPass() const
{
	return mRenderPass;
}

void RenderPassAsset::_load()
{
	const std::string fileContent = FileSystem::instance().loadToString(mPath);

	const auto jsonValue = nlohmann::json::parse(fileContent);

	const auto& attachments = jsonValue["attachments"];
	const auto& descriptions = jsonValue["descriptions"];
	const auto& dependencies = jsonValue["dependencies"];

	for (const auto& attachment : attachments)
	{
		AttachmentDescription desc = {};

		uint32_t format = attachment["format"];
		uint32_t samples = attachment["samples"];
		uint32_t loadOp = attachment["load"];
		uint32_t storeOp = attachment["store"];
		uint32_t loadStencilOp = attachment["loadstencil"];
		uint32_t storeStencilOp = attachment["storestencil"];
		uint32_t initialFormat = attachment["initial"];
		uint32_t finalFormat = attachment["final"];

		desc.format = static_cast<EDataFormat>(format);
		desc.samples = static_cast<EImageSampleFlagBits>(samples);
		desc.load = static_cast<EAttachmentLoadOperation>(loadOp);
		desc.store = static_cast<EAttachmentStoreOperation>(storeOp);
		desc.loadStencil = static_cast<EAttachmentLoadOperation>(loadStencilOp);
		desc.storeStencil = static_cast<EAttachmentStoreOperation>(storeStencilOp);
		desc.initial = static_cast<EImageLayout>(initialFormat);
		desc.final = static_cast<EImageLayout>(finalFormat);

		mCreateInfo.attachments.push_back(desc);
	}

	for (const auto& description : descriptions)
	{
		SubPassDescription desc = {};

		const auto& inputs = description["inputs"];
		const auto& colors = description["colors"];
		const auto& resolves = description["resolves"];
		const auto& depth = description["depth"];
		const auto& preserves = description["preserve"];

		for (const auto& input : inputs)
		{
			desc.inputs.push_back(_process(std::addressof(input)));
		}

		for (const auto& color : colors)
		{
			desc.colors.push_back(_process(std::addressof(color)));
		}

		for (const auto& resolve : resolves)
		{
			desc.resolve.push_back(_process(std::addressof(resolve)));
		}

		desc.depthStencil = _process(std::addressof(depth));

		for (const uint32_t preserve : preserves)
		{
			desc.preserve.push_back(preserve);
		}

		mCreateInfo.descriptions.push_back(desc);
	}

	for (const auto & dependency : dependencies)
	{
		uint32_t srcSubPass = dependency["srcsubpass"];
		uint32_t dstSubPass = dependency["dstsubpass"];
		uint32_t srcStages = dependency["srcstages"];
		uint32_t dstStages = dependency["dststages"];
		uint32_t srcAccess = dependency["srcaccess"];
		uint32_t dstAccess = dependency["dstaccess"];
		uint32_t dependencyFlags = dependency["dependencyflags"];

		SubPassDependency dep = {};
		dep.srcSubPass = srcSubPass;
		dep.dstSubPass = dstSubPass;
		dep.srcStages = srcStages;
		dep.dstStages = dstStages;
		dep.srcAccess = srcAccess;
		dep.dstAccess = dstAccess;
		dep.dependencyFlags = dependencyFlags;

		mCreateInfo.dependencies.push_back(dep);
	}

	mRenderPass = GraphicsFactory::instance().createRenderPass();
	mRenderPass->construct(mCreateInfo);
}

AttachmentReference RenderPassAsset::_process(const void* aData) const
{
	const nlohmann::json& value = *reinterpret_cast<const nlohmann::json*>(aData);

	AttachmentReference ref;

	const uint32_t index = value["index"];
	const uint32_t layout = value["layout"];

	ref.index = index;
	ref.layout = static_cast<EImageLayout>(layout);

	return ref;
}
