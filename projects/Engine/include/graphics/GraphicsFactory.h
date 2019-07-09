#ifndef graphicsfactory_h__
#define graphicsfactory_h__

#include "graphics/api/IDescriptorSet.h"
#include "graphics/api/IFramebuffer.h"
#include "graphics/api/IGraphicsPipeline.h"
#include "graphics/api/IIndexBuffer.h"
#include "graphics/api/IPipelineLayout.h"
#include "graphics/api/IRenderPass.h"
#include "graphics/api/IShaderModule.h"
#include "graphics/api/IShaderStage.h"
#include "graphics/api/IUniformBuffer.h"
#include "graphics/api/IVertexBuffer.h"

#include <vector>

enum ERenderAPI : uint32_t
{
	RENDERAPI_VULKAN = 0,
	RENDERAPI_NONE
};

class GraphicsFactory
{
	public:
		static GraphicsFactory& instance();
		void initialize(uint32_t aAPI, IGraphicsContext* aContext);

		[[nodiscard]] IDescriptorSet* createDescriptorSet() const;
		[[nodiscard]] IDescriptorSetLayout* createDescriptorSetLayout() const;
		[[nodiscard]] IFramebuffer* createFramebuffer() const;
		[[nodiscard]] IGraphicsPipeline* createGraphicsPipeline() const;
		[[nodiscard]] IIndexBuffer* createIndexBuffer() const;
		[[nodiscard]] IPipelineLayout* createPipelineLayout() const;
		[[nodiscard]] IRenderPass* createRenderPass() const;
		[[nodiscard]] IShaderModule* createShaderModule() const;
		[[nodiscard]] IShaderStage* createShaderStage() const;
		[[nodiscard]] IUniformBuffer* createUniformBuffer() const;
		[[nodiscard]] IVertexBuffer* createVertexBuffer() const;

	private:
		GraphicsFactory();

		uint32_t mAPI;
		IGraphicsContext* mContext;
};

#endif // graphicsfactory_h__