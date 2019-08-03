#ifndef gamelayer_h__
#define gamelayer_h__

#include <Primal.h>
#include "assets/ShaderAsset.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/DescriptorSetPool.h"
#include "graphics/UniformBufferPool.h"

class GameLayer : public ApplicationLayer
{
	public:
		GameLayer();
		~GameLayer();

		void onAttach() override;

		void onUpdate() override;
		void onRender() override;

		void onDetach() override;

		void onEvent(Event& aEvent) override;

	private:
		std::shared_ptr<ShaderAsset> mShaderAsset;
		IGraphicsPipeline* mGraphicsPipeline;
		VulkanPipelineLayout* mLayout;
		DescriptorSetPool* mDescPool;
		UniformBufferPool* mUboPool;
};

#endif // gamelayer_h__