#include "graphics/Material.h"

#include "core/PrimalCast.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanTexture.h"

Material::Material(IGraphicsPipeline& aPipeline, const std::vector<UniformBufferObject*>& aUBOs, const std::vector<ITexture*>& aTextures, Material* aParentMaterial)
	: mPipeline(aPipeline), mUBOs(aUBOs), mTextures(aTextures)
{
	mSet = GraphicsFactory::instance().createDescriptorSet();

	uint32_t i = 0;
	for (const auto ubo : mUBOs)
	{
		void* data = malloc(ubo->getSize());
		mData.push_back({ data, ubo->getSize() });

		for (const auto element : ubo->getElements())
		{
			mVariableData.insert({ element->name, {i, {element, ubo}} });
		}
		++i;
	}
}

Material::~Material()
{
	delete mSet;
	for (const auto ubo : mData)
	{
		free(ubo.first);
	}
}

bool Material::isInstanced() const
{
	return mParentMaterial != nullptr;
}