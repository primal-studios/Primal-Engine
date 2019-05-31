#ifndef vulkancommandpool_h__
#define vulkancommandpool_h__

#include "graphics/api/ICommandPool.h"

#include <vulkan/vulkan.h>

class VulkanCommandPool final : public ICommandPool
{
	public:
		explicit VulkanCommandPool(IGraphicsContext* aContext);
		VulkanCommandPool(const VulkanCommandPool&) = delete;
		VulkanCommandPool(VulkanCommandPool&&) noexcept = delete;
		~VulkanCommandPool() override;

		VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
		VulkanCommandPool& operator=(VulkanCommandPool&&) noexcept = delete;

		void construct(const CommandPoolCreateInfo& aInfo) override;
		VkCommandPool getPool() const;
	private:
		VkCommandPool mPool{};
};

#endif // vulkancommandpool_h__
