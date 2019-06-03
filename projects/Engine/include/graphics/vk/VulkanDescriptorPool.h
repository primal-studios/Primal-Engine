#ifndef vulkandescriptorpool_h__
#define vulkandescriptorpool_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IDescriptorPool.h"

class VulkanDescriptorPool final : public IDescriptorPool
{
	public:
		explicit VulkanDescriptorPool(IGraphicsContext* aContext);
		VulkanDescriptorPool(const VulkanDescriptorPool&) = delete;
		VulkanDescriptorPool(VulkanDescriptorPool&&) noexcept = delete;
		~VulkanDescriptorPool() override;

		VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;
		VulkanDescriptorPool& operator=(VulkanDescriptorPool&&) noexcept = delete;

		void construct(const DescriptorPoolCreateInfo& aInfo) override;

		VkDescriptorPool getHandle() const;

	private:
		VkDescriptorPool mPool;
};

#endif // vulkandescriptorpool_h__