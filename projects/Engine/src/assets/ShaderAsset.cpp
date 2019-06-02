#include "assets/ShaderAsset.h"
#include "filesystem/FileSystem.h"

#include <json/json.hpp>

ShaderAsset::ShaderAsset(const std::string& aPath)
{
	mGraphicsPipelineCreateInfo = {};
	mPath = aPath;
}

ShaderAsset::~ShaderAsset()
{
	
}

void ShaderAsset::_load()
{
	const std::string fileContent = FileSystem::instance().loadToString(mPath);

	const auto jsonValue = nlohmann::json::parse(fileContent);


}