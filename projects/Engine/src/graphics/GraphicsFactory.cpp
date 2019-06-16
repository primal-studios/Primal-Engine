#include "graphics/GraphicsFactory.h"
#include "graphics/vk/VulkanVertexBuffer.h"
#include "graphics/vk/VulkanIndexBuffer.h"

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

GraphicsFactory::GraphicsFactory() 
	: mAPI(ERenderAPI::RENDERAPI_NONE), mContext(nullptr)
{

}
