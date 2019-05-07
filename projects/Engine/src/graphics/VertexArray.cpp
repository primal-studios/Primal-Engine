#include "graphics/VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray()
{
	mIndexBuffers = nullptr;
	glGenVertexArrays(1, &mId);
}

VertexArray::VertexArray(VertexBuffer* aVertexBuffer, IndexBuffer* aIndexBuffer)
	: VertexArray()
{
	pushVertexBuffer(aVertexBuffer);
	setIndexBuffer(aIndexBuffer);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &mId);
}

void VertexArray::pushVertexBuffer(VertexBuffer* aBuffer)
{
	mVertexBuffers.push_back(aBuffer);;

	bind();
	aBuffer->bind();

	_applyLayout(aBuffer->getLayout());

	unbind();
	aBuffer->unbind();
}

void VertexArray::setIndexBuffer(IndexBuffer* aBuffer)
{
	mIndexBuffers = aBuffer;

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

VertexBuffer* VertexArray::getVertexBuffer(const uint32_t aIndex) const
{
	if(aIndex >= mVertexBuffers.size())
	{
		PRIMAL_INTERNAL_ERROR("Buffer does not exist at index: {0}", aIndex);
		return nullptr;
	}

	return mVertexBuffers[aIndex];
}

IndexBuffer* VertexArray::getIndexBuffer() const
{
	return mIndexBuffers;
}

void VertexArray::_applyLayout(const VertexBufferLayout& aLayout) const
{
	const std::vector<VertexBufferLayoutElement>& elements = aLayout.getLayout();
	for(size_t i = 0; i < elements.size(); i++)
	{
		const VertexBufferLayoutElement& element = elements[i];
		glEnableVertexAttribArray(static_cast<uint32_t>(i));
		glVertexAttribPointer(static_cast<uint32_t>(i), element.count, element.type, element.normalized, static_cast<GLsizei>(aLayout.getStride()), reinterpret_cast<const void*>(element.offset));
	}
}
