#include "graphics/GraphicsFactory.h"
#include "graphics/vk/VulkanVertexBuffer.h"
#include "graphics/vk/VulkanIndexBuffer.h"
#include "graphics/vk/VulkanUniformBuffer.h"

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

GraphicsFactory::GraphicsFactory() 
	: mAPI(ERenderAPI::RENDERAPI_NONE), mContext(nullptr)
{

}
