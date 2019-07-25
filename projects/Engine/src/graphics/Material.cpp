#include <utility>
#include "graphics/Material.h"

#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanTexture.h"

Material::Material(const MaterialCreateInfo& aCreateInfo)
	: mCreateInfo(aCreateInfo), mPipeline(aCreateInfo.pipeline), mTextures(aCreateInfo.textures), mUboLayouts(aCreateInfo.layouts)
{
	mSet = aCreateInfo.pool->acquire();

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
		auto descSetLayout = VulkanTexture::getDescriptorSetLayout(tex.second->getBindingPoint(), tex.second->getStageFlags(), 0);
		bindings.push_back(descSetLayout);
	}

	mLayout->construct({ 0, bindings });

	DescriptorSetCreateInfo setCreateInfo = {};
	setCreateInfo.pool = aCreateInfo.pool->getPool(mSet);
	setCreateInfo.setLayouts.push_back(mLayout);
	setCreateInfo.setLayouts.push_back(mLayout);

	mSet.set->construct(setCreateInfo);
	mDescSetPool = aCreateInfo.pool;
}

Material::~Material()
{
	mDescSetPool->release(mSet);
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

Material* Material::clone()
{
	const auto mat = new Material(mCreateInfo);
	Material* m = this;
	while (m->mParent != nullptr)
	{
		m = m->mParent;
	}
	mat->mParent = m;
	return mat;
}

MaterialInstance* Material::createInstance()
{
	Material* mat = mParent != nullptr ? mParent : this;

	if (mat->mFreeSlots.empty())
	{
		std::vector<UniformBufferObject*> ubos;
		for (const auto& ubo : mat->mUboLayouts)
		{
			UniformBufferObject* obj = ubo->acquire(mat->mCursor);
			ubos.push_back(obj);

			const auto backingBuffer = mat->mBackingBuffers.find(ubo);
			BackingBufferInfo bufInfo = {};
			if (backingBuffer == mat->mBackingBuffers.end())
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
			if (mat->mCursor < bufferCapacity)
			{
				bufInfo.blocks.push_back(malloc(BufferSize));
			}

			if (backingBuffer != mat->mBackingBuffers.end())
			{
				mat->mBackingBuffers.erase(ubo);
			}

			mat->mBackingBuffers.insert({ ubo, bufInfo });
		}
		return new MaterialInstance(this, ubos, mat->mCursor++);
	}
	const size_t idx = *mat->mFreeSlots.begin();
	mat->mFreeSlots.erase(mat->mFreeSlots.begin());
	std::vector<UniformBufferObject*> ubos;
	for (const auto& ubo : mat->mUboLayouts)
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
	++mParent->mChildInstances;
}

MaterialInstance::~MaterialInstance()
{
	mParent->mFreeSlots.push_back(mInstanceId);
	--mParent->mChildInstances;
}

Material* MaterialInstance::setTexture(std::string aName, ITexture* aTexture)
{
	Material* mat = mParent;
	if (mParent->mChildInstances > 1)
	{
		mat = new Material(mParent->mCreateInfo);
		mat->mParent = mParent;
		mParent = mat;
	}
	mat->mTextures[aName] = aTexture;
	return mat;
}

std::pair<UniformBufferPool*, UniformBufferObjectElement*> MaterialInstance::_getElementFromName(const std::string& aName) const
{
	auto parent = mParent;

	const auto it = parent->mElements.find(aName);
	if (it == parent->mElements.end())
	{
		return {nullptr, nullptr};
	}
	return it->second;
}
