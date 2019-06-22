#ifndef graphicsfactory_h__
#define graphicsfactory_h__

#include "api/IVertexBuffer.h"
#include "api/IIndexBuffer.h"
#include "api/IUniformBuffer.h"

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

		IVertexBuffer* createVertexBuffer() const;
		IIndexBuffer* createIndexBuffer() const;
		IUniformBuffer* createUniformBuffer() const;

	private:
		GraphicsFactory();

		uint32_t mAPI;
		IGraphicsContext* mContext;
};

#endif // graphicsfactory_h__