#include "assets/MeshAsset.h"

#include <fx/gltf.h>

#include "utils/StringUtils.h"

MeshAsset::MeshAsset(const std::string& aPath)
{
	mPath = aPath;
}

void MeshAsset::_load()
{
	const bool isText = StringUtils::endsWith(mPath, "gltf");
	const bool isBinary = StringUtils::endsWith(mPath, "glb");

	fx::gltf::Document document;

	if (isText)
	{
		document = fx::gltf::LoadFromText(mPath);
	}

	if(isBinary)
	{
		document = fx::gltf::LoadFromBinary(mPath);
	}

	size_t sceneCount = document.scenes.size();
	size_t meshCount = document.meshes.size();
	size_t textureCount = document.textures.size();
	size_t imageCount = document.images.size();
	size_t materialCount = document.materials.size();
	size_t cameraCount = document.cameras.size();
	size_t nodeCount = document.nodes.size();
	size_t skinCount = document.skins.size();
	size_t samplerCount = document.samplers.size();
	size_t animationCount = document.animations.size();
	size_t accessorCount = document.accessors.size();
	size_t bufferCount = document.buffers.size();
	size_t bufferViewCount = document.bufferViews.size();

	int32_t jonathan = 0;
}
