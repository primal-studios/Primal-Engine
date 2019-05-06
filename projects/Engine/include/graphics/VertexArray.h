#ifndef vertexarray_h__
#define vertexarray_h__

#include <vector>

#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

class VertexArray
{
	public:
		VertexArray();
		VertexArray(VertexBuffer* aVertexBuffer, IndexBuffer* aIndexBuffer);
		~VertexArray();

		void pushVertexBuffer(VertexBuffer* aBuffer);
		void pushIndexBuffer(IndexBuffer* aBuffer);

		void bind() const;
		void unbind() const;

		VertexBuffer* getVertexBuffer(const uint32_t aIndex = 0) const;
		IndexBuffer* getIndexBuffer(const uint32_t aIndex = 0) const;

	private:
		uint32_t mId;

		std::vector<VertexBuffer*> mVertexBuffers;
		std::vector<IndexBuffer*> mIndexBuffers;

		void _applyLayout(const VertexBufferLayout& aLayout) const;
};

#endif // vertexarray_h__