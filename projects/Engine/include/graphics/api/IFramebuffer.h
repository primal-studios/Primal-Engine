#ifndef iframebuffer_h__
#define iframebuffer_h__

#include <graphics/api/IGraphicsContext.h>
#include <graphics/api/IImageView.h>
#include <graphics/api/IRenderPass.h>

#include <cstdint>
#include <vector>

struct FramebufferCreateInfo
{
	std::vector<IImageView*> attachments;
	IRenderPass* renderPass;
	uint32_t width;
	uint32_t height;
	uint32_t layers;
};

class IFramebuffer
{
public:
	explicit IFramebuffer(IGraphicsContext* aContext);
	IFramebuffer(const IFramebuffer&) = delete;
	IFramebuffer(IFramebuffer&&) noexcept = delete;
	virtual ~IFramebuffer() = default;

	IFramebuffer& operator=(const IFramebuffer&) = delete;
	IFramebuffer& operator=(IFramebuffer&&) noexcept = delete;

	virtual void construct(const FramebufferCreateInfo&) = 0;
};

#endif // iframebuffer_h__
