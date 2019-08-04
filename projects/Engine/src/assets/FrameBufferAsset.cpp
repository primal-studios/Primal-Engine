#include "assets/FrameBufferAsset.h"
#include <json/json.hpp>
#include "filesystem/FileSystem.h"
#include "assets/AssetManager.h"
#include "assets/RenderPassAsset.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

FrameBufferAsset::FrameBufferAsset(const std::string& aFile)
	: mFrameBuffer(nullptr), mRenderPass(nullptr)
{
	mFileName = aFile;
}

FrameBufferAsset::~FrameBufferAsset()
{
	
}

IFramebuffer* FrameBufferAsset::getFramebuffer() const
{
	return mFrameBuffer;
}

IRenderPass* FrameBufferAsset::getRenderPass() const
{
	return mRenderPass;
}


void FrameBufferAsset::_load()
{
	const std::string fileContent = FileSystem::instance().loadToString(mFileName);
	const auto jsonValue = nlohmann::json::parse(fileContent);

	const auto images = jsonValue["images"];
	const auto imageViews = jsonValue["imageViews"];

	const auto framebuffer = jsonValue["frameBuffer"];
	const std::string renderPassName = framebuffer["renderPass"];

	const auto renderPassAsset = AssetManager::instance().get<RenderPassAsset>(renderPassName);

	for(auto image : images)
	{
		uint32_t flags = image["flags"];
		uint32_t type = image["type"];
		uint32_t format = image["format"];
		Vector3f extent = Vector3f(image["extent"][0], image["extent"][1], image["extent"][2]);
		uint32_t mipLevels = image["mipLevels"];
		uint32_t arrayLayers = image["arrayLayers"];
		uint32_t samples = image["samples"];
		uint32_t tiling = image["tiling"];
		uint32_t usage = image["usage"];
		uint32_t sharingMode = image["sharingMode"];
		uint32_t queueFamilyIndexCount = image["queueFamilyIndexCount"];
		std::vector<uint32_t> queueFamilyIndices;
		for(const auto& index : image["queueFamilyIndices"])
		{
			queueFamilyIndices.push_back(index);
		}
		uint32_t initialLayout = image["initialLayout"];
	}

	for(auto imageView : imageViews)
	{
		uint32_t flags = imageView["flags"];
		uint32_t image = imageView["image"];
		uint32_t viewType = imageView["viewType"];
		uint32_t format = imageView["format"];
		Vector4f components = Vector4f(imageView["components"][0], imageView["components"][1], imageView["components"][2], imageView["components"][3]);

		uint32_t aspectMask = imageView["subresourceRange"]["aspectMask"];
		uint32_t baseMipLevel = imageView["subresourceRange"]["baseMipLevel"];
		uint32_t levelCount = imageView["subresourceRange"]["levelCount"];
		uint32_t baseArrayLayer = imageView["subresourceRange"]["baseArrayLayer"];
		uint32_t layerCount = imageView["subresourceRange"]["layerCount"];
	}

	uint32_t flags = framebuffer["flags"];
	uint32_t attachmentCount = framebuffer["attachmentCount"];
	std::vector<uint32_t> attachments;
	for (const auto& attachment : framebuffer["attachments"])
	{
		attachments.push_back(attachment);
	}

	uint32_t width = framebuffer["width"];
	uint32_t height = framebuffer["height"];
	uint32_t layers = framebuffer["layers"];

	int jonathan = 0;
}


