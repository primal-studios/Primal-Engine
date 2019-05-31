#ifndef vulkandescriptorsetlayout_h__
#define vulkandescriptorsetlayout_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IDescriptorSetLayout.h"

class VulkanDescriptorSetLayout final : public IDescriptorSetLayout
{
	public:
		explicit VulkanDescriptorSetLayout(IGraphicsContext* aContext);
		VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout&) = delete;
		VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&&) noexcept = delete;
		virtual ~VulkanDescriptorSetLayout() override;

		VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout&) = delete;
		VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&&) noexcept = delete;

		void construct(const DescriptorSetLayoutCreateInfo& aInfo) override;
		void reconstruct(const DescriptorSetLayoutCreateInfo& aInfo) override;

		VkDescriptorSetLayout getHandle() const;
	private:
		void _destroy() const;

		VkDescriptorSetLayout mLayout{};
};

#endif // vulkandescriptorsetlayout_h__