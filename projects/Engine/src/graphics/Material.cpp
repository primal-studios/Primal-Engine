#include "graphics/Material.h"

#include "core/PrimalCast.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"

Material::Material(IGraphicsPipeline& aPipeline, const std::vector<UniformBufferObject*> aUBOs, Material* aParentMaterial)
{
	mSet = GraphicsFactory::instance().createDescriptorSet();
}

Material::~Material()
{

}

bool Material::isInstanced() const
{
	return mParentMaterial != nullptr;
}

void Material::use(ICommandBuffer* aBuffer)
{
	VulkanCommandBuffer* buffer = primal_cast<VulkanCommandBuffer*>(aBuffer);
	VkCommandBuffer handle = buffer->getHandle();

	std::vector<VkWriteDescriptorSet> writeSets;

	for (auto ubo : mUBOs)
	{
		VulkanUniformBuffer* ub = primal_cast<VulkanUniformBuffer*>(ubo->getBuffer());
		std::optional<OffsetSize> offset = OffsetSize(ubo->getOffset(), ubo->getSize());
		WriteDescriptorSet desc = ub->getWriteDescriptor(ubo->getBindingPoint(), offset);
	}
}
