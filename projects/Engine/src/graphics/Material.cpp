#include "graphics/Material.h"

#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanTexture.h"

Material::Material(IGraphicsPipeline& aPipeline, IDescriptorPool* aPool, const std::vector<UniformBufferObject*>& aUBOs, const std::vector<ITexture*>& aTextures, Material* aParentMaterial)
	: mPipeline(aPipeline), mUBOs(aUBOs), mTextures(aTextures)
{
	mSet = GraphicsFactory::instance().createDescriptorSet();

	mLayout = GraphicsFactory::instance().createDescriptorSetLayout();
	std::vector<DescriptorSetLayoutBinding> bindings;
	for (const auto& ubo : aUBOs)
	{
		// TODO: add mechanism to set shader stage mask
		auto descSetLayout = VulkanUniformBuffer::getDescriptorSetLayout(ubo->getBindingPoint(), VK_SHADER_STAGE_VERTEX_BIT, 1);
		bindings.push_back(descSetLayout);
	}

	for (const auto& tex : aTextures)
	{
		auto descSetLayout = VulkanTexture::getDescriptorSetLayout(tex->getBindingPoint(), tex->getStageFlags(), 0);
		bindings.push_back(descSetLayout);
	}

	DescriptorSetCreateInfo setCreateInfo = {};
	mLayout->construct({ 0, bindings });
	setCreateInfo.setLayouts.push_back(mLayout);
	setCreateInfo.setLayouts.push_back(mLayout);
	setCreateInfo.pool = aPool;

	mSet->construct(setCreateInfo);

	setCreateInfo.pool = aPool;
	setCreateInfo.setLayouts.push_back(mLayout);
	setCreateInfo.setLayouts.push_back(mLayout);

	mSet->construct(setCreateInfo);

	uint32_t i = 0;
	for (const auto ubo : mUBOs)
	{
		void* data = malloc(ubo->getSize());
		mData.push_back({ data, ubo->getSize() });

		for (const auto element : ubo->getElements())
		{
			mVariableData.insert({ element->name, {ubo->getBindingPoint(), {element, ubo}} });
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

	delete mLayout;
}

bool Material::isInstanced() const
{
	return mParentMaterial != nullptr;
}