#include "graphics/VertexBuffer.h"
#include "glad/glad.h"

VertexBuffer::VertexBuffer(const void* aData, const size_t aSize)
{
	mSize = aSize;
	glGenBuffers(1, &mId);
	glBindBuffer(GL_ARRAY_BUFFER, mId);
	glBufferData(GL_ARRAY_BUFFER, mSize, aData, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &mId);
}

void VertexBuffer::setLayout(const BufferLayout& aLayout)
{
	mLayout = aLayout;
}

void VertexBuffer::bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, mId);
}

void VertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}