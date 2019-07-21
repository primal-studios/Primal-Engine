#ifndef vertexbuffer_h__
#define vertexbuffer_h__

#include "graphics/BufferLayout.h"

class VertexBuffer
{
	public:
		VertexBuffer(const void* aData, const size_t aSize);
		~VertexBuffer();

		void setLayout(const BufferLayout& aLayout);
		inline const BufferLayout& getLayout() const { return mLayout; }

		void bind() const;
		void unbind() const;

	private:
		size_t mSize;
		uint32_t mId;

		BufferLayout mLayout;
};

#endif // vertexbuffer_h__