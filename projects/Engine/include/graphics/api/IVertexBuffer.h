#ifndef ivertexbuffer_h__
#define ivertexbuffer_h__

#include <cstdint>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/VertexBufferLayout.h"

enum EVertexBufferCreateFlagBits : uint32_t
{
	VERTEX_BUFFER_STATIC = 0x01,
	VERTEX_BUFFER_DYNAMIC = 0x02,
	VERTEX_BUFFER_INSTANCED = 0x04,
	VERTEX_BUFFER_SHARING_MODE_CONCLUSIVE = 0x08,
	VERTEX_BUFFER_SHARING_MODE_EXCLUSIVE = 0x10
};

using VertexBufferCreateFlags = uint32_t;

class IVertexBuffer
{
	public:
		explicit IVertexBuffer(IGraphicsContext* aContext);
		IVertexBuffer(const IVertexBuffer& aOther) = delete;
		IVertexBuffer(IVertexBuffer&& aOther) noexcept = delete;
		virtual ~IVertexBuffer() = default;

		IVertexBuffer& operator = (const IVertexBuffer& aOther) = delete;
		IVertexBuffer& operator = (IVertexBuffer&& aOther) noexcept = delete;

		virtual void construct(const VertexBufferCreateFlags& aInfo) = 0;

		virtual void setData(void* aData, const size_t aSize) = 0;
		virtual void setLayout(const VertexBufferLayout& aLayout) = 0;

		virtual void bind() = 0;
		virtual void unbind() = 0;

	protected:
		IGraphicsContext* mContext;

		size_t mSize;
		void* mData;

		VertexBufferLayout mLayout;
};

#endif // ivertexbuffer_h__