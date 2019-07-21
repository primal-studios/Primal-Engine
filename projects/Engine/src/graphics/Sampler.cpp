#include "graphics/Sampler.h"
#include "glad/glad.h"

Sampler::Sampler()
{
	glGenSamplers(1, &mId);
}

Sampler::~Sampler()
{
	glDeleteSamplers(1, &mId);
}

void Sampler::setParameteri(ESamplerParameter aParam, const int32_t aValue) const
{
	glSamplerParameteri(mId, static_cast<GLenum>(aParam), aValue);
}

void Sampler::setParameteriv(ESamplerParameter aParam, int32_t* aValue) const
{
	glSamplerParameterIiv(mId, static_cast<GLenum>(aParam), aValue);
}

void Sampler::setParameterf(ESamplerParameter aParam, const float aValue) const
{
	glSamplerParameterf(mId, static_cast<GLenum>(aParam), aValue);
}

void Sampler::setParameterfv(ESamplerParameter aParam, float* aValue) const
{
	glSamplerParameterfv(mId, static_cast<GLenum>(aParam), aValue);
}

void Sampler::bind(const uint32_t aUnit) const
{
	glBindSampler(aUnit, mId);
}

uint32_t Sampler::getId() const
{
	return mId;
}
