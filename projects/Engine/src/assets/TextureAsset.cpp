#include "assets/TextureAsset.h"
#include "core/Types.h"
#include "filesystem/FileSystem.h"
#include "assets/AssetManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "core/Log.h"
#include <json/json.hpp>
#include "assets/SamplerAsset.h"
#include "graphics/vk/VulkanTexture.h"
#include "graphics/GraphicsFactory.h"

TextureAsset::TextureAsset(const std::string& aPath, const uint32_t aDesiredChannels)
{
	mPath = aPath;
	mDesiredChannels = aDesiredChannels;
	mFile = {};
}

TextureAsset::~TextureAsset()
{
	delete mTexture;
}

ImageFile TextureAsset::getData() const
{
	return mFile;
}

ITexture* TextureAsset::getTexture() const
{
	return mTexture;
}

ISampler* TextureAsset::getSampler() const
{
	return mSampler;
}

void TextureAsset::_load()
{
	Path loadPath = FileSystem::instance().getMountedDirectory();

	const std::string jsonData = FileSystem::instance().loadToString(mPath);
	const auto jsonValue = nlohmann::json::parse(jsonData);

	const std::string samplerName = jsonValue["samplername"];
	auto sampler = AssetManager::instance().load<SamplerAsset>(samplerName, samplerName);

	mSampler = sampler->getSampler();

	uint32_t bindingPoint = jsonValue["bindingpoint"];
	uint32_t shaderStage = jsonValue["shaderstage"];

	const std::string textureFileName = jsonValue["texturefile"];
	loadPath += textureFileName;

	int x, y, channels;
	auto widePath = loadPath.native();

	std::vector<char> convertedPath;
	convertedPath.reserve(widePath.size());

	std::string s;

	for (const auto& c : widePath)
	{
		if (c > 255)
		{
			PRIMAL_INTERNAL_ERROR("Invalid character code in path.");
			return;
		}
		convertedPath.push_back(static_cast<char>(c));
		s += static_cast<char>(c);
	}

	unsigned char* payload = stbi_load(s.c_str(), &x, &y, &channels, mDesiredChannels);
	std::vector<unsigned char> results;
	results.resize(static_cast<size_t>(x) * static_cast<size_t>(y) * mDesiredChannels);
	memcpy(results.data(), payload, results.size());
	stbi_image_free(payload);

	mFile.payload = results;
	mFile.bitsPerPixel = 8;
	mFile.channels = mDesiredChannels;
	mFile.width = x;
	mFile.height = y;

	TextureCreateInfo textureCreateInfo = {};
	textureCreateInfo.sampler = mSampler;
	textureCreateInfo.textureAsset = this;
	textureCreateInfo.binding = bindingPoint;
	textureCreateInfo.shaderStageAccess = shaderStage;

	mTexture = GraphicsFactory::instance().createTexture();
	mTexture->construct(textureCreateInfo);
}
