#ifndef icommandbuffer_h__
#define icommandbuffer_h__

#include <graphics/api/ICommandPool.h>
#include <graphics/api/IGraphicsContext.h>

struct CommandBufferCreateInfo
{
	ICommandPool* pool;
	bool primary;
};

class ICommandBuffer
{
	public:
		explicit ICommandBuffer(IGraphicsContext* aContext);
		ICommandBuffer(const ICommandBuffer&) = delete;
		ICommandBuffer(ICommandBuffer&&) noexcept = delete;
		virtual ~ICommandBuffer() = default;

		ICommandBuffer& operator=(const ICommandBuffer&) = delete;
		ICommandBuffer& operator=(ICommandBuffer&&) noexcept = delete;

		virtual void addDependency(ICommandBuffer*) = 0;
		virtual void removeDependency(ICommandBuffer*) = 0;

		virtual void construct(const CommandBufferCreateInfo&) = 0;
		virtual void reconstruct(const CommandBufferCreateInfo&) = 0;
};

#endif // icommandbuffer_h__
