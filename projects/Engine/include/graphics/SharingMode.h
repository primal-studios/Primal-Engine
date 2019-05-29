#ifndef sharingmode_h__
#define sharingmode_h__

#include <cstdint>

enum class ESharingMode : uint32_t
{
	EXCLUSIVE  = 0x0,
	CONCURRENT = 0x1
};

#endif // sharingmode_h__
