#ifndef vulkandescriptorset_h__
#define vulkandescriptorset_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IDescriptorSet.h"
#include <memory>

class WriteDescriptorSet
{
	public:
		WriteDescriptorSet(const VkWriteDescriptorSet& aWriteDescriptorSet, const VkDescriptorImageInfo& aImageInfo)
			: mWriteDescriptorSet(aWriteDescriptorSet)
		{
			mWriteDescriptorSet.pImageInfo = new VkDescriptorImageInfo(aImageInfo);
		}

		WriteDescriptorSet(const VkWriteDescriptorSet& aWriteDescriptorSet, const VkDescriptorBufferInfo& aImageInfo)
			: mWriteDescriptorSet(aWriteDescriptorSet)
		{
			mWriteDescriptorSet.pBufferInfo = new VkDescriptorBufferInfo(aImageInfo);
		}

		~WriteDescriptorSet()
		{
			delete mImageInfo;
			delete mBufferInfo;
		}

		const VkWriteDescriptorSet& getWriteDescriptorSet() const { return mWriteDescriptorSet; }

	private:
		VkWriteDescriptorSet mWriteDescriptorSet;
		VkDescriptorImageInfo* mImageInfo = nullptr;
		VkDescriptorBufferInfo* mBufferInfo = nullptr;
};

class VulkanDescriptorSet final : public IDescriptorSet
{
	public:
		explicit VulkanDescriptorSet(IGraphicsContext* aContext);
		VulkanDescriptorSet(const VulkanDescriptorSet&) = delete;
		VulkanDescriptorSet(VulkanDescriptorSet&&) noexcept = delete;
		~VulkanDescriptorSet() override;

		VulkanDescriptorSet& operator=(const VulkanDescriptorSet&) = delete;
		VulkanDescriptorSet& operator=(VulkanDescriptorSet&&) noexcept = delete;

		void construct(const DescriptorSetCreateInfo& aInfo) override;

		VkDescriptorSet getHandle(const uint32_t aIndex) const;

	private:
		std::vector<VkDescriptorSet> mSets;
};

#endif // vulkandescriptorset_h__