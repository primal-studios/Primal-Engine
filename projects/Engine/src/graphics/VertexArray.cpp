#include "graphics/VertexArray.h"

#include "glad/glad.h"

#include "graphics/VertexBuffer.h"
#include "graphics/IndexBuffer.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &mId);
}

VertexArray::VertexArray(VertexBuffer* aVertexBuffer, IndexBuffer* aIndexBuffer)
{
	glGenVertexArrays(1, &mId);
	addVertexBuffer(aVertexBuffer);
	addIndexBuffer(aIndexBuffer);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &mId);
}

void VertexArray::addVertexBuffer(VertexBuffer* aBuffer)
{
	mVertexBuffers.push_back(aBuffer);

	bind();
	aBuffer->bind();

	_applyLayout(aBuffer->getLayout());

	unbind();
	aBuffer->unbind();

}

void VertexArray::addIndexBuffer(IndexBuffer* aBuffer)
{
	mIndexBuffers.push_back(aBuffer);

	bind();
	aBuffer->bind();
	unbind();
	aBuffer->unbind();
}

void VertexArray::bind() const
{
	glBindVertexArray(mId);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::_applyLayout(const BufferLayout& aLayout) const
{
	const std::vector<BufferLayoutElement>& elements = aLayout.getLayout();
	for (size_t i = 0; i < elements.size(); i++)
	{
		const BufferLayoutElement& element = elements[i];
		glEnableVertexAttribArray(static_cast<uint32_t>(i));
		glVertexAttribPointer(static_cast<uint32_t>(i), element.count, static_cast<GLenum>(element.type), element.normalized, static_cast<GLsizei>(aLayout.getStride()), reinterpret_cast<const void*>(element.offset));
	}
}