#ifndef vertexarray_h__
#define vertexarray_h__

#include <cstdint>
#include <vector>

class VertexBuffer;
class BufferLayout;
class IndexBuffer;

class VertexArray
{
	public:
		VertexArray();
		VertexArray(VertexBuffer* aVertexBuffer, IndexBuffer* aIndexBuffer);
		~VertexArray();

		void addVertexBuffer(VertexBuffer* aBuffer);
		void addIndexBuffer(IndexBuffer* aBuffer);

		void bind() const;
		void unbind() const;

		VertexBuffer* getVertexBuffer(const uint32_t aIndex = 0) const { return mVertexBuffers[aIndex]; }
		IndexBuffer* getIndexBuffer(const uint32_t aIndex = 0) const { return mIndexBuffers[aIndex]; }

	private:
		uint32_t mId;

		std::vector<VertexBuffer*> mVertexBuffers;
		std::vector<IndexBuffer*> mIndexBuffers;

		void _applyLayout(const BufferLayout& aLayout) const;
};

#endif // vertexarray_h__