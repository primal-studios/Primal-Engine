#ifndef vulkandescriptorset_h__
#define vulkandescriptorset_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IDescriptorSet.h"

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