#include "graphics/MaterialManager.h"

MaterialManager MaterialManager::sManager;

MaterialManager& MaterialManager::instance()
{
	return sManager;
}

void MaterialManager::prune(Material* aMaterial)
{
	detail::PruneNode(aMaterial->mGraphNode);
}

void MaterialManager::pruneAll()
{
	for (auto & material : mRootMaterials)
	{
		prune(material);
	}
}

void MaterialManager::reset()
{
	for (auto & material : mRootMaterials)
	{
		detail::ResetNode(material->mGraphNode);
	}
	mRootMaterials.clear();
}

Material* MaterialManager::createMaterial(const MaterialCreateInfo& aInfo)
{
	return new Material(aInfo);
}
