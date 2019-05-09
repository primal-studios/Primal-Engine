#ifndef rendersystem_h__
#define rendersystem_h__

#include "core/Window.h"
#include "ecs/System.h"
#include "graphics/api/IGraphicsContext.h"

class RenderSystem final : public System
{
	public:
		explicit RenderSystem(Window* aWindow);
		~RenderSystem();

		void initialize() override;

		void preRender() override;
		void render() override;
		void postRender() override;
	private:
		IGraphicsContext* mContext;
};

#endif // rendersystem_h__