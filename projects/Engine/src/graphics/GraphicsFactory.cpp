#include "graphics/GraphicsFactory.h"
#include "graphics/vk/VulkanDescriptorSetLayout.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanRenderPass.h"
#include "graphics/vk/VulkanShaderModule.h"
#include "graphics/vk/VulkanShaderStage.h"
#include "graphics/vk/VulkanUniformBuffer.h"
#include "graphics/vk/VulkanVertexBuffer.h"

GraphicsFactory& GraphicsFactory::instance()
{
	static GraphicsFactory* instance = new GraphicsFactory();
	return *instance;
}

void GraphicsFactory::initialize(uint32_t aAPI, IGraphicsContext* aContext)
{
	mAPI = aAPI;
	mContext = aContext;
}

IShaderModule* GraphicsFactory::createShaderModule() const
{
	switch (mAPI)
	{
		case ERenderAPI::RENDERAPI_VULKAN:
		{
			VulkanShaderModule* module = new VulkanShaderModule(mContext);
			return module;
		}
		case ERenderAPI::RENDERAPI_NONE:
		{
			return nullptr;
		}
	}
	return nullptr;
}

IShaderStage* GraphicsFactory::createShaderStage() const
{
	switch (mAPI)
	{
	case ERenderAPI::RENDERAPI_VULKAN:
	{
		VulkanShaderStage* stage = new VulkanShaderStage(mContext);
		return stage;
	}
	case ERenderAPI::RENDERAPI_NONE:
	{
		return nullptr;
	}
	}
	return nullptr;
}

IVertexBuffer* GraphicsFactory::createVertexBuffer() const
{
	switch(mAPI)
	{
		case ERenderAPI::RENDERAPI_VULKAN:
		{
			VulkanVertexBuffer* buffer = new VulkanVertexBuffer(mContext);
			return buffer;
		}

		case ERenderAPI::RENDERAPI_NONE:
		{
			return nullptr;
		}
	}

	return nullptr;
}

IIndexBuffer* GraphicsFactory::createIndexBuffer() const
{
	switch (mAPI)
	{
		case ERenderAPI::RENDERAPI_VULKAN:
		{
			VulkanIndexBuffer* buffer = new VulkanIndexBuffer(mContext);
			return buffer;
		}

		case ERenderAPI::RENDERAPI_NONE:
		{
			return nullptr;
		}
	}

	return nullptr;
}

IPipelineLayout* GraphicsFactory::createPipelineLayout() const
{
	switch (mAPI)
	{
		case ERenderAPI::RENDERAPI_VULKAN:
		{
			VulkanPipelineLayout* layout = new VulkanPipelineLayout(mContext);
			return layout;
		}

		case ERenderAPI::RENDERAPI_NONE:
		{
			return nullptr;
		}
	}

	return nullptr;
}

IUniformBuffer* GraphicsFactory::createUniformBuffer() const
{
	switch (mAPI)
	{
		case RENDERAPI_VULKAN:
		{
			return new VulkanUniformBuffer(mContext);
		}
		case RENDERAPI_NONE:
		{
			return nullptr;
		}
		default:
		{
			return nullptr;
		}
	}
}

IDescriptorSet* GraphicsFactory::createDescriptorSet() const
{
	switch (mAPI)
	{
		case RENDERAPI_VULKAN:
		{
			return new VulkanDescriptorSet(mContext);
		}
		case RENDERAPI_NONE:
		{
			return nullptr;
		}
		default:
		{
			return nullptr;
		}
	}
}

IRenderPass* GraphicsFactory::createRenderPass() const
{
	switch (mAPI)
	{
		case RENDERAPI_VULKAN:
		{
			VulkanRenderPass* renderPass = new VulkanRenderPass(mContext);
			return renderPass;
		}
		case RENDERAPI_NONE:
		{
			return nullptr;
		}
	}
	return nullptr;
}

IDescriptorSetLayout* GraphicsFactory::createDescriptorSetLayout() const
{
	switch (mAPI)
	{
		case RENDERAPI_VULKAN:
		{
			VulkanDescriptorSetLayout* setLayout = new VulkanDescriptorSetLayout(mContext);
			return setLayout;
		}
		case RENDERAPI_NONE:
		{
			return nullptr;
		}
	}
	return nullptr;
}

IGraphicsPipeline* GraphicsFactory::createGraphicsPipeline() const
{
	switch (mAPI)
	{
		case RENDERAPI_VULKAN:
		{
			VulkanGraphicsPipeline* pipeline = new VulkanGraphicsPipeline(mContext);
			return pipeline;
		}
		case RENDERAPI_NONE:
		{
			return nullptr;
		}
	}
	return nullptr;
}

GraphicsFactory::GraphicsFactory() 
	: mAPI(ERenderAPI::RENDERAPI_NONE), mContext(nullptr)
{

}
