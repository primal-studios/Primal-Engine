#ifndef rendersystem_h__
#define rendersystem_h__

#include "core/Window.h"
#include "ecs/System.h"

#include "graphics/vk/VulkanCommandPool.h"
#include "graphics/vk/VulkanGraphicsContext.h"

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
	private:
		VulkanGraphicsContext* mContext;
		VulkanCommandPool* mPool;
};

#endif // rendersystem_h__