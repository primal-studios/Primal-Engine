#include "graphics/Texture.h"

#include <glad/glad.h>

Texture::Texture()
	: Texture(0, 0, ETextureInternalFormat::UNKNOWN, ETextureFormat::UNKNOWN)
{
	
}

Texture::Texture(const uint32_t aWidth, const uint32_t aHeight, const ETextureInternalFormat aInternal, const ETextureFormat aFormat)
	: mId(0), mWidth(aWidth), mHeight(aHeight), mFormat(aFormat), mInternal(aInternal)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mId);
}

Texture::~Texture()
{
	glDeleteTextures(1, &mId);
}

void Texture::setData(std::vector<void*> aData, const uint32_t aWidth, const uint32_t aHeight, ETextureInternalFormat aInternal,
	ETextureFormat aFormat, ETextureDataType aDataType)
{
	glTextureStorage2D(mId, aData.size(), static_cast<GLenum>(aInternal), aWidth, aHeight);
	for (size_t i = 0; i < aData.size(); i++)
	{
		glTextureSubImage2D(mId, i, 0, 0, aWidth, aHeight, static_cast<GLenum>(aFormat), static_cast<GLenum>(aDataType), aData[i]);
	}

	mWidth = aWidth;
	mHeight = aHeight;
	mInternal = aInternal;
	mFormat = aFormat;
}

void Texture::bind(const uint32_t aLocation) const
{
	glBindTextureUnit(aLocation, mId);
}

uint32_t Texture::getWidth() const
{
	return mWidth;
}

uint32_t Texture::getHeight() const
{
	return mHeight;
}

uint32_t Texture::getId() const
{
	return mId;
}

ETextureFormat Texture::getFormat() const
{
	return mFormat;
}
