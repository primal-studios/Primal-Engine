#include <utility>
#include "graphics/Material.h"

#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanTexture.h"

Material::Material(const MaterialCreateInfo& aCreateInfo)
	: mPipeline(aCreateInfo.pipeline), mUboLayouts(aCreateInfo.layouts), mTextures(aCreateInfo.textures)
{
	mSet = GraphicsFactory::instance().createDescriptorSet();

	mLayout = GraphicsFactory::instance().createDescriptorSetLayout();
	std::vector<DescriptorSetLayoutBinding> bindings;
	for (const auto& ubo : aCreateInfo.layouts)
	{
		// TODO: add mechanism to set shader stage mask
		auto descSetLayout = VulkanUniformBuffer::getDescriptorSetLayout(ubo->getBindingPoint(), VK_SHADER_STAGE_VERTEX_BIT, 1);
		bindings.push_back(descSetLayout);

		for (const auto& element : ubo->getElements())
		{
			mElements.insert({ element->name, { ubo, element } });
		}
	}

	for (const auto& tex : aCreateInfo.textures)
	{
		auto descSetLayout = VulkanTexture::getDescriptorSetLayout(tex->getBindingPoint(), tex->getStageFlags(), 0);
		bindings.push_back(descSetLayout);
	}

	mLayout->construct({ 0, bindings });

	DescriptorSetCreateInfo setCreateInfo = {};
	setCreateInfo.pool = aCreateInfo.pool;
	setCreateInfo.setLayouts.push_back(mLayout);
	setCreateInfo.setLayouts.push_back(mLayout);

	mSet->construct(setCreateInfo);
}

Material::~Material()
{
	delete mSet;
	delete mLayout;

	for (const auto& buf : mBackingBuffers)
	{
		for (const auto block : buf.second.blocks)
		{
			free(block);
		}
	}
}

static constexpr size_t BufferSize = 65536;

MaterialInstance* Material::createInstance()
{
	if (mFreeSlots.empty())
	{
		std::vector<UniformBufferObject*> ubos;
		for (const auto& ubo : mUboLayouts)
		{
			UniformBufferObject* obj = ubo->acquire(mCursor);
			ubos.push_back(obj);

			const auto backingBuffer = mBackingBuffers.find(ubo);
			BackingBufferInfo bufInfo = {};
			if (backingBuffer == mBackingBuffers.end())
			{
				bufInfo.blocks.push_back(malloc(BufferSize));
				bufInfo.elementCount = BufferSize / obj->getSize();
				bufInfo.elementSize = obj->getSize();
			}
			else
			{
				bufInfo = backingBuffer->second;
			}

			const size_t bufferCapacity = bufInfo.blocks.size() * bufInfo.elementCount;
			if (mCursor < bufferCapacity)
			{
				bufInfo.blocks.push_back(malloc(BufferSize));
			}

			if (backingBuffer != mBackingBuffers.end())
			{
				mBackingBuffers.erase(ubo);
			}

			mBackingBuffers.insert({ ubo, bufInfo });
		}
		return new MaterialInstance(this, ubos, mCursor++);
	}
	const size_t idx = *mFreeSlots.begin();
	mFreeSlots.erase(mFreeSlots.begin());
	std::vector<UniformBufferObject*> ubos;
	for (const auto& ubo : mUboLayouts)
	{
		ubos.push_back(ubo->acquire(idx));
	}
	return new MaterialInstance(this, ubos, idx);
}

void Material::destroyInstance(MaterialInstance* aInstance)
{
	delete aInstance;
}

MaterialInstance::MaterialInstance(Material* aMaterial, std::vector<UniformBufferObject*> aUbos, size_t aInstanceId)
	: mParent(aMaterial), mUbos(std::move(aUbos)), mInstanceId(aInstanceId)
{
}

MaterialInstance::~MaterialInstance()
{
	mParent->mFreeSlots.push_back(mInstanceId);
}

std::pair<UniformBufferPool*, UniformBufferObjectElement*> MaterialInstance::_getElementFromName(const std::string& aName) const
{
	const auto it = mParent->mElements.find(aName);
	if (it == mParent->mElements.end())
	{
		return {nullptr, nullptr};
	}
	return it->second;
}
