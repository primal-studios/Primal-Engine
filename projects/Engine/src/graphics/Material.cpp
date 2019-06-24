#include "graphics/Material.h"

Material::Material(IGraphicsPipeline& aPipeline, const std::vector<UniformBufferObject*> aUBOs, Material* aParentMaterial)
{
	
}

Material::~Material()
{

}

bool Material::isInstanced() const
{
	return mParentMaterial != nullptr;
}
