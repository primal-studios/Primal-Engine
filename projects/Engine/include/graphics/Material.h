#ifndef material_h__
#define material_h__

#include "core/PrimalCast.h"

#include "graphics/UniformBufferPool.h"
#include "graphics/api/IDescriptorSet.h"
#include "graphics/api/IGraphicsPipeline.h"
#include "graphics/api/ITexture.h"

class Material
{
	friend class VulkanCommandBuffer;

	public:
		Material(IGraphicsPipeline& aPipeline, const std::vector<UniformBufferObject*>& aUBOs, const std::vector<ITexture*>& aTextures, Material* aParentMaterial = nullptr);
		Material(const Material&) = delete;
		Material(Material&&) noexcept = delete;
		~Material();

		Material& operator=(const Material&) = delete;
		Material& operator=(Material&&) noexcept = delete;

		[[nodiscard]] bool isInstanced() const;
		
		template <typename T>
		void setVariable(const std::string& aName, T aValue);

		template <typename T>
		T getVariable(const std::string& aName);

	private:
		IGraphicsPipeline& mPipeline;
		Material* mParentMaterial = nullptr;
		std::vector<std::pair<void*, uint32_t>> mData;

		IDescriptorSet* mSet;
		std::unordered_map<std::string, std::pair<uint32_t, std::pair<UniformBufferObjectElement*, UniformBufferObject*>>> mVariableData;
		std::vector<UniformBufferObject*> mUBOs;
		std::vector<ITexture*> mTextures;
};

template <typename T>
void Material::setVariable(const std::string& aName, T aValue)
{
	const auto it = mVariableData.find(aName);
	if (it == mVariableData.end())
	{
		PRIMAL_ERROR("Could not find variable named: {}", aName.c_str());
	}
	else
	{
		const uint32_t buf = it->second.first;
		std::pair<UniformBufferObjectElement*, UniformBufferObject*> bufferInfo = it->second.second;
		char* buffer = static_cast<char*>(mData[buf].first);
		memcpy(buffer + bufferInfo.first->offset, &aValue, bufferInfo.first->size);
	}
}

template <typename T>
T Material::getVariable(const std::string& aName)
{
	// TODO: getting the variable from the correct buffer
	return T();
}

#endif // material_h__
