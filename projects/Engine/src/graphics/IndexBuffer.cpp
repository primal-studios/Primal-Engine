#include "graphics/IndexBuffer.h"

#include <glad/glad.h>

IndexBuffer::IndexBuffer(uint32_t* aData, const size_t aSize)
{
	mCount = static_cast<uint32_t>(aSize / sizeof(uint32_t));

	glGenBuffers(1, &mId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, aSize, aData, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &mId);
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
}

void IndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

uint32_t IndexBuffer::count() const
{
	return mCount;
}
