#include <json/json.hpp>

#include "assets/SamplerAsset.h"
#include "graphics/GraphicsFactory.h"
#include "filesystem/FileSystem.h"

SamplerAsset::SamplerAsset(const std::string& aFile)
	: mFileName(aFile)
{
}

SamplerAsset::~SamplerAsset()
{
	delete mSampler;
}

ISampler* SamplerAsset::getSampler() const
{
	return mSampler;
}

void SamplerAsset::_load()
{
	const std::string& contents = FileSystem::instance().loadToString(mFileName);
	nlohmann::json json = nlohmann::json::parse(contents);

	const uint32_t flags = json["flags"];
	const uint32_t magFilter = json["magfilter"];
	const uint32_t minFilter = json["minfilter"];
	const uint32_t mipMapMode = json["mipmapmode"];
	const uint32_t addressModeU = json["addressmodeu"];
	const uint32_t addressModeV = json["addressmodev"];
	const uint32_t addressModeW = json["addressmodew"];
	const float mipLodBias = json["miplodbias"];
	const bool anisotropyEnable = json["anisotropyenable"];
	const float maxAnisotropy = json["maxanisotropy"];
	const bool compareEnable = json["compareenable"];
	const uint32_t compareOp = json["compareop"];
	const float minLod = json["minlod"];
	const float maxLod = json["maxlod"];
	const uint32_t borderColor = json["bordercolor"];
	const bool unnormalizedCoordinates = json["unnormalizedcoordinates"];

	const SamplerCreateInfo createInfo = {
		flags,
		static_cast<EFilter>(magFilter),
		static_cast<EFilter>(minFilter),
		static_cast<ESamplerMipmapMode>(mipMapMode),
		static_cast<ESamplerAddressMode>(addressModeU),
		static_cast<ESamplerAddressMode>(addressModeV),
		static_cast<ESamplerAddressMode>(addressModeW),
		mipLodBias,
		anisotropyEnable,
		maxAnisotropy,
		compareEnable,
		static_cast<ECompareOp>(compareOp),
		minLod,
		maxLod,
		static_cast<EBorderColor>(borderColor),
		unnormalizedCoordinates
	};

	mSampler = GraphicsFactory::instance().createSampler();
	mSampler->construct(createInfo);
}
