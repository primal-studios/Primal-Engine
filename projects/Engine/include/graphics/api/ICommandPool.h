#ifndef icommandpool_h__
#define icommandpool_h__

#include "graphics/api/IGraphicsContext.h"

#include <cstdint>

enum ECommandPoolCreateFlagBits : uint32_t
{
	COMMAND_POOL_CREATE_TRANSIENT		= 0x01,
	COMMAND_POOL_RESET_COMMAND_BUFFER	= 0x02,
	COMMAND_POOL_CREATE_PROTECTED		= 0x04
};

using CommandPoolCreateFlags = uint32_t;

struct CommandPoolCreateInfo
{
	CommandPoolCreateFlags flags;
	uint32_t queueFamilyIndex;
};

class ICommandPool
{
	public:
		explicit ICommandPool(IGraphicsContext* aContext);
		ICommandPool(const ICommandPool&) = delete;
		ICommandPool(ICommandPool&&) noexcept = delete;
		virtual ~ICommandPool() = default;

		ICommandPool& operator=(const ICommandPool&) = delete;
		ICommandPool& operator=(ICommandPool&&) noexcept = delete;

		virtual void construct(const CommandPoolCreateInfo& aInfo) = 0;
	protected:
		IGraphicsContext* mContext;
};

#endif // icommandpool_h__
