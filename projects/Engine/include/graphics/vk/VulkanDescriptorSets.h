#ifndef vulkandescriptorset_h__
#define vulkandescriptorset_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IDescriptorSets.h"

class VulkanDescriptorSets final : public IDescriptorSets
{
	public:
		explicit VulkanDescriptorSets(IGraphicsContext* aContext);
		VulkanDescriptorSets(const VulkanDescriptorSets&) = delete;
		VulkanDescriptorSets(VulkanDescriptorSets&&) noexcept = delete;
		~VulkanDescriptorSets() override;

		VulkanDescriptorSets& operator=(const VulkanDescriptorSets&) = delete;
		VulkanDescriptorSets& operator=(VulkanDescriptorSets&&) noexcept = delete;

		void construct(const DescriptorSetCreateInfo& aInfo) override;

		VkDescriptorSet getSet(const size_t aIndex) const;

	private:
		std::vector<VkDescriptorSet> mSets;
};

#endif // vulkandescriptorset_h__