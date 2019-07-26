#ifndef scenedata_h__
#define scenedata_h__

#include "graphics/api/IDescriptorSet.h"
#include "graphics/api/IDescriptorPool.h"
#include "graphics/UniformBufferPool.h"

struct SceneDataCreateInfo
{
	std::vector<UniformBufferObjectElement*> elements;
	IPipelineLayout* layout;
};

class SceneData
{
		friend class VulkanCommandBuffer;
	public:
		explicit SceneData(const SceneDataCreateInfo& aInfo);
		SceneData(const SceneData&) = delete;
		SceneData(SceneData&&) noexcept = delete;
		~SceneData();

		SceneData& operator=(const SceneData&) = delete;
		SceneData& operator=(SceneData&&) noexcept = delete;

		template<typename T>
		void setValue(const std::string& aName, T aValue);

		template<typename T>
		T* getValue(const std::string& aName);

	private:
		SceneDataCreateInfo mInfo;
		IDescriptorSet* mSet = nullptr;
		IDescriptorSetLayout* mSetLayout = nullptr;
		IDescriptorPool* mDescSetPool = nullptr;
		UniformBufferPool* mUboPool = nullptr;
		UniformBufferObject* mObj = nullptr;

		std::unordered_map<std::string, size_t> mOffsets;
		void* mCpuBacking = nullptr;
		size_t mBackingSz = 0;
};

template <typename T>
void SceneData::setValue(const std::string& aName, T aValue)
{
	T* ptr = getValue<T>(aName);
	memcpy(ptr, &aValue, sizeof(T));
}

template <typename T>
inline T* SceneData::getValue(const std::string& aName)
{
	if (mOffsets.find(aName) != mOffsets.end())
	{
		return reinterpret_cast<T*>(reinterpret_cast<char*>(mCpuBacking) + mOffsets[aName]);
	}
	return nullptr;
}

#endif // scenedata_h__
