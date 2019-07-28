#include "graphics/Material.h"

#include "graphics/MaterialManager.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanTexture.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace detail
{
	static void SanitizeMaterialNode(MaterialGraphNode* aNode);
	static void MarkMaterialAsDirty(MaterialGraphNode* aNode);
}

struct MaterialGraphNode
{
	Material* materialHandle = nullptr;
	std::vector<MaterialGraphNode*> children;
	MaterialGraphNode* parent = nullptr;
	uint32_t instances = 0;

	MaterialGraphNode() = default;
	MaterialGraphNode(const MaterialGraphNode&) = delete;
	MaterialGraphNode(MaterialGraphNode&&) noexcept = delete;
	MaterialGraphNode& operator=(const MaterialGraphNode&) = delete;
	MaterialGraphNode& operator=(MaterialGraphNode&&) noexcept = delete;

	~MaterialGraphNode()
	{
		detail::SanitizeMaterialNode(this);
	}

	void markMaterialDirty()
	{
		detail::MarkMaterialAsDirty(this);
	}
};

namespace detail
{
	static void SanitizeMaterialNode(MaterialGraphNode* aNode)
	{
		for (const auto & child : aNode->children)
		{
			child->parent = nullptr;
		}

		if (aNode->parent)
		{
			const auto it = std::find(aNode->parent->children.begin(), aNode->parent->children.end(), aNode);
			if (it != aNode->parent->children.end())
			{
				aNode->parent->children.erase(it);
			}
		}
	}

	static void AddMaterialChild(MaterialGraphNode* aParent, MaterialGraphNode* aChild)
	{
		SanitizeMaterialNode(aChild);
		aParent->children.push_back(aChild);
		aChild->parent = aParent;
	}

	static Material* GetParent(MaterialGraphNode* aNode)
	{
		if (aNode->parent)
		{
			return aNode->parent->materialHandle;
		}
		return nullptr;
	}

	static Material* GetFirstAncestor(MaterialGraphNode* aNode)
	{
		auto node = aNode;
		while (node->parent != nullptr)
		{
			node = node->parent;
		}
		return node->materialHandle;
	}

	static void MarkMaterialAsDirty(MaterialGraphNode* aNode)
	{
		for (const auto & node : aNode->children)
		{
			MarkMaterialAsDirty(node);
		}
		aNode->materialHandle->setDirtyFlag(true);
	}

	void PruneNode(MaterialGraphNode* aNode)
	{
		// PRUNE IF HAS SINGLE CHILD OR NO CHILDREN

		// ON PRUNE
		// - Distribute the state information to child
		// - Remove node from tree
		// - Move children to parent


		if (aNode->instances != 0)
		{
			return;
		}

		if (aNode->children.size() == 1)
		{
			auto child = aNode->children[0];
			// Distribute to child
			if (!aNode->parent)
			{
				// Root node. Move UBO information to child
				child->materialHandle->mUboLayouts = aNode->materialHandle->mUboLayouts;
				aNode->materialHandle->mUboLayouts.clear();

				auto& instance = MaterialManager::instance();
				instance.mRootMaterials.erase(std::find(instance.mRootMaterials.begin(), instance.mRootMaterials.end(), aNode->materialHandle));
				instance.mRootMaterials.push_back(child->materialHandle);
			}

			for (const auto& textureIt : aNode->materialHandle->mTextures)
			{
				auto mat = child->materialHandle;
				if (mat->mTextures.find(textureIt.first) == mat->mTextures.end())
				{
					mat->mTextures.insert({ textureIt.first, textureIt.second });
				}
			}

			aNode->materialHandle->mTextures.clear();

			delete aNode->materialHandle;
		}
		else if (aNode->children.empty())
		{
			delete aNode->materialHandle;
		}
		else
		{
			for (auto child : aNode->children)
			{
				PruneNode(child);
			}
		}
	}

	void ResetNode(MaterialGraphNode* aNode)
	{
		auto children = aNode->children;
		for (auto child : children)
		{
			ResetNode(child);
		}
		delete aNode->materialHandle;
	}
}

Material::Material(const MaterialCreateInfo& aCreateInfo, bool aIsRoot)
	: mCreateInfo(aCreateInfo), mPipeline(aCreateInfo.pipeline), mTextures(aCreateInfo.textures), mUboLayouts(aCreateInfo.layouts)
{
	mSet = aCreateInfo.pool->acquire();

	mLayout = GraphicsFactory::instance().createDescriptorSetLayout();
	std::vector<DescriptorSetLayoutBinding> bindings;
	for (const auto& ubo : aCreateInfo.layouts)
	{
		// TODO: add mechanism to set shader stage mask
		auto descSetLayout = VulkanUniformBuffer::getDescriptorSetLayout(ubo->getBindingPoint(), VK_SHADER_STAGE_ALL_GRAPHICS, 1);
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

	mGraphNode = new MaterialGraphNode;
	mGraphNode->materialHandle = this;
	this->setDirtyFlag(true);

	if (aIsRoot)
	{
		MaterialManager::instance().mRootMaterials.push_back(this);
	}
}

Material::Material(const MaterialCreateInfo& aCreateInfo)
	: Material(aCreateInfo, true)
{
}

Material::~Material()
{
	delete mGraphNode;
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

Material* Material::clone() const
{
	const auto mat = new Material(mCreateInfo, false);

	mat->mTextures.clear();
	mat->mUboLayouts.clear();

	detail::AddMaterialChild(this->mGraphNode, mat->mGraphNode);
	return mat;
}

void Material::setDirtyFlag(const bool aIsDirty)
{
	mDirtyBit = aIsDirty ? 2 : 0;
}

void Material::setTexture(const std::string& aName, ITexture* aTexture)
{
	mTextures[aName] = aTexture;
	detail::MarkMaterialAsDirty(mGraphNode);
}

MaterialInstance* Material::createInstance()
{
	Material* mat = detail::GetParent(mGraphNode);
	if (mat == nullptr)
	{
		mat = mGraphNode->materialHandle;
	}

	if (mat->mFreeSlots.empty())
	{
		std::vector<UniformBufferObject*> ubos;
		for (const auto& ubo : mat->mUboLayouts)
		{
			UniformBufferObject* obj = ubo->acquire(static_cast<uint32_t>(mat->mCursor));
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
			if (mat->mCursor >= bufferCapacity)
			{
				bufInfo.blocks.push_back(malloc(BufferSize));
			}

			if (backingBuffer != mat->mBackingBuffers.end())
			{
				mat->mBackingBuffers.erase(ubo);
			}

			mat->mBackingBuffers[ubo] = bufInfo;
		}
		return new MaterialInstance(this, ubos, mat->mCursor++);
	}
	const size_t idx = *mat->mFreeSlots.begin();
	mat->mFreeSlots.erase(mat->mFreeSlots.begin());
	std::vector<UniformBufferObject*> ubos;
	for (const auto& ubo : mat->mUboLayouts)
	{
		ubos.push_back(ubo->acquire(static_cast<uint32_t>(idx)));
	}
	return new MaterialInstance(this, ubos, idx);
}

void Material::destroyInstance(MaterialInstance* aInstance)
{
	delete aInstance;
}

void Material::_markDirty() const
{
	this->mGraphNode->markMaterialDirty();
}

std::vector<ITexture*> Material::_getActiveTextures() const
{
	std::unordered_map<uint32_t, ITexture*> textures;
	const Material* mat = this;
	while (mat != nullptr)
	{
		for (auto & tex : mat->mTextures)
		{
			if (textures.find(tex.second->getBindingPoint()) == textures.end())
			{
				textures.insert({ tex.second->getBindingPoint(), tex.second });
			}
		}
		mat = detail::GetParent(mat->mGraphNode);
	}

	std::vector<ITexture*> res;
	for (const auto & it : textures)
	{
		res.push_back(it.second);
	}

	return res;
}

Material* Material::_getRootAncestor() const
{
	return detail::GetFirstAncestor(mGraphNode);
}

Material* Material::_getParent() const
{
	return detail::GetParent(mGraphNode);
}

MaterialInstance::MaterialInstance(Material* aMaterial, std::vector<UniformBufferObject*> aUbos, size_t aInstanceId)
	: mParent(aMaterial), mUbos(std::move(aUbos)), mInstanceId(aInstanceId)
{
	++mParent->mGraphNode->instances;
}

MaterialInstance::~MaterialInstance()
{
	mParent->mFreeSlots.push_back(mInstanceId);
	--mParent->mGraphNode->instances;
}

Material* MaterialInstance::setTexture(const std::string& aName, ITexture* aTexture)
{
	Material* mat = mParent;
	if (mParent->mGraphNode->instances > 1)
	{
		--mParent->mGraphNode->instances;
		mat = mParent->clone();
		mat->mTextures.insert({ aName, aTexture });
		mParent = mat;
	}
	else
	{
		mat->mTextures[aName] = aTexture;
		mat->setDirtyFlag(true);
	}
	return mat;
}

Material* MaterialInstance::getParentMaterial() const
{
	return mParent;
}

std::pair<UniformBufferPool*, UniformBufferObjectElement*> MaterialInstance::_getElementFromName(const std::string& aName) const
{
	auto parent = detail::GetFirstAncestor(mParent->mGraphNode);

	const auto it = parent->mElements.find(aName);
	if (it == parent->mElements.end())
	{
		return {nullptr, nullptr};
	}
	return it->second;
}
