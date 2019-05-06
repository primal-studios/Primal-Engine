#ifndef vertexbuffer_h__
#define vertexbuffer_h__

#include <cstdint>
#include "graphics/VertexBufferLayout.h"

class VertexBuffer
{
	public:
		VertexBuffer(const void* aData, const size_t aSize);
		~VertexBuffer();

		void setLayout(const VertexBufferLayout& aLayout);
		const VertexBufferLayout& getLayout() const { return mLayout; }

		void bind() const;
		void unbind() const;

	private:
		uint32_t mId;

		size_t mSize;
		VertexBufferLayout mLayout;
};

#endif // vertexbuffer_h__