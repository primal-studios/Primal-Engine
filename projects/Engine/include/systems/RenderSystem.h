#ifndef rendersystem_h__
#define rendersystem_h__

#include "ecs/System.h"

class RenderSystem final : public System
{
	public:
		RenderSystem();
		~RenderSystem();

		void initialize() override;

		void preRender() override;
		void render() override;
		void postRender() override;
};

#endif // rendersystem_h__