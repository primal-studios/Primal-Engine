#ifndef material_h__
#define material_h__

#include "graphics/DescriptorSetPool.h"
#include "graphics/UniformBufferPool.h"
#include "graphics/api/IDescriptorSet.h"
#include "graphics/api/IGraphicsPipeline.h"
#include "graphics/api/ITexture.h"

struct MaterialCreateInfo
{
	DescriptorSetPool* pool;
	IGraphicsPipeline* pipeline;
	std::vector<UniformBufferPool*> layouts;
	std::unordered_map<std::string, ITexture*> textures;
};

class MaterialInstance;
class Material;
struct MaterialGraphNode;

namespace detail {
	void PruneNode(MaterialGraphNode* aNode);
	void ResetNode(MaterialGraphNode* aNode);
}

class Material
{
	friend void detail::PruneNode(MaterialGraphNode* aNode);

	friend struct MaterialGraphNode;
	friend class MaterialInstance;
	friend class MaterialManager;
	friend class VulkanCommandBuffer;

	struct BackingBufferInfo
	{
		std::vector<void*> blocks;
		size_t elementCount;
		size_t elementSize;
	};
		explicit Material(const MaterialCreateInfo& aCreateInfo);
		explicit Material(const MaterialCreateInfo& aCreateInfo, bool aIsRoot);
	public:
		Material(const Material&) = delete;
		Material(Material&&) noexcept = delete;
		~Material();

		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) noexcept = delete;

		MaterialInstance* createInstance();
		void destroyInstance(MaterialInstance* aInstance);

		[[nodiscard]] Material* clone() const;
		void setDirtyFlag(bool aIsDirty);

		void setTexture(const std::string& aName, ITexture* aTexture);
	private:
		MaterialCreateInfo mCreateInfo;
		IGraphicsPipeline* mPipeline;
		DescriptorSet mSet{};
		DescriptorSetPool* mDescSetPool;
		IDescriptorSetLayout* mLayout = nullptr;
		std::unordered_map<std::string, ITexture*> mTextures;
		std::vector<UniformBufferPool*> mUboLayouts;

		std::unordered_map<UniformBufferPool*, BackingBufferInfo> mBackingBuffers;
		std::vector<size_t> mFreeSlots;
		uint64_t mCursor = 0;

		std::unordered_map<std::string, std::pair<UniformBufferPool*, UniformBufferObjectElement*>> mElements;

		uint8_t mDirtyBit = 2;

		MaterialGraphNode* mGraphNode = nullptr;

		void _markDirty() const;

		std::vector<ITexture*> _getActiveTextures() const;
		Material* _getRootAncestor() const;
		Material* _getParent() const;
};

class MaterialInstance
{
	friend class Material;
	friend class VulkanCommandBuffer;

	Material* mParent;
	std::vector<UniformBufferObject*> mUbos;
	size_t mInstanceId;

	explicit MaterialInstance(Material* aMaterial, std::vector<UniformBufferObject*> aUbos, size_t aInstanceId);
public:
	MaterialInstance(const MaterialInstance&) = delete;
	MaterialInstance(MaterialInstance&&) noexcept = delete;
	~MaterialInstance();

	MaterialInstance& operator=(const MaterialInstance&) = delete;
	MaterialInstance& operator=(MaterialInstance&&) noexcept = delete;

	template<typename T>
	void setVariable(const std::string& aName, T aValue);

	template<typename T>
	T* getVariable(const std::string& aName);

	Material* setTexture(const std::string& aName, ITexture* aTexture);
	Material* getParentMaterial() const;

private:
	[[nodiscard]] std::pair<UniformBufferPool*, UniformBufferObjectElement*> _getElementFromName(const std::string& aName) const;
};

template <typename T>
void MaterialInstance::setVariable(const std::string& aName, T aValue)
{
	memcpy(getVariable<T>(aName), &aValue, sizeof(T));
}

template <typename T>
T* MaterialInstance::getVariable(const std::string& aName)
{
	auto parent = mParent->_getRootAncestor();

	const auto pair = _getElementFromName(aName);
	PRIMAL_ASSERT(pair.first != nullptr && pair.second != nullptr, "Invalid variable");
	const Material::BackingBufferInfo& buf = parent->mBackingBuffers[pair.first];
	const size_t blockIndex = mInstanceId / buf.elementCount;
	const size_t offset = ((mInstanceId - (blockIndex * buf.elementCount)) * buf.elementCount) + pair.second->offset;
	char* block = reinterpret_cast<char*>(buf.blocks[blockIndex]);
	return reinterpret_cast<T*>(block + offset);
}

#endif // material_h__
