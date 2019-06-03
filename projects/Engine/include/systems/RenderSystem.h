#ifndef rendersystem_h__
#define rendersystem_h__

#include "core/Window.h"
#include "ecs/System.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanFramebuffer.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanRenderPass.h"
#include "graphics/vk/VulkanSwapChain.h"
#include "graphics/api/IVertexBuffer.h"
#include "graphics/api/IIndexBuffer.h"

#include "events/ApplicationEvent.h"
#include "graphics/api/IUniformBuffer.h"

class RenderSystem final : public System
{
	public:
		explicit RenderSystem(Window* aWindow);
		RenderSystem(const RenderSystem&) = delete;
		RenderSystem(RenderSystem&&) noexcept = delete;
		~RenderSystem();

		RenderSystem& operator=(const RenderSystem&) = delete;
		RenderSystem& operator=(RenderSystem&&) noexcept = delete;

		void initialize() override;

		void preRender() override;
		void render() override;
		void postRender() override;

		void onEvent(Event& aEvent) override;
	private:
		VulkanGraphicsContext* mContext;
		VulkanSwapChain* mSwapChain;
		VulkanCommandPool* mPool;
		VulkanRenderPass* mRenderPass;
		const uint32_t mFlightSize = 2;
		uint32_t mCurrentFrame = 0;

		IGraphicsPipeline* mGraphicsPipeline;
		IPipelineLayout* mLayout;

		VulkanCommandBuffer** mPrimaryBuffer = nullptr;
		VulkanFramebuffer** mFramebuffers = nullptr;

		IVertexBuffer* mVertexBuffer;
		IVertexBuffer* mColorBuffer;
		IIndexBuffer* mIndexBuffer;
		IUniformBuffer* mUniformBuffer;
		IDescriptorPool* mDescriptorPool;
		IDescriptorSets* mSets;
		IDescriptorSetLayout* mDescLayout;

		CommandBufferInheritanceInfo mPrimaryInheritance = {};
		CommandBufferRecordInfo mPrimaryRecordInfo = {};

		Window* mWindow;

		bool _onResize(WindowResizeEvent& aEvent);
};

#endif // rendersystem_h__