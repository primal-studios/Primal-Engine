#ifndef vulkanrendersystem_h__
#define vulkanrendersystem_h__

#include "ecs/System.h"
#include "core/Window.h"

#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanSwapChain.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanRenderPass.h"
#include "graphics/vk/VulkanFramebuffer.h"

class VulkanRenderSystem : public System
{
	public:
		explicit VulkanRenderSystem(Window* aWindow);
		VulkanRenderSystem(const VulkanRenderSystem&) = delete;
		VulkanRenderSystem(VulkanRenderSystem&&) noexcept = delete;
		~VulkanRenderSystem();

		VulkanRenderSystem& operator=(const VulkanRenderSystem&) = delete;
		VulkanRenderSystem& operator=(VulkanRenderSystem&&) noexcept = delete;

		void initialize() override;

		void preRender() override;
		void render() override;
		void postRender() override;

		void onEvent(Event& aEvent) override;

	private:
		VulkanGraphicsContext* mContext = nullptr;
		VulkanSwapChain* mSwapChain = nullptr;
		VulkanRenderPass* mRenderPass = nullptr;
		Window* mWindow = nullptr;

		std::vector<VulkanCommandBuffer*> mCommandBuffers;
		std::vector<VulkanFramebuffer*> mFrameBuffers;

		const uint32_t mFlightSize = 2;
		uint32_t mCurrentFrame = 0;

};

#endif // vulkanrendersystem_h__