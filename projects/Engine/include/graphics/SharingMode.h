#ifndef sharingmode_h__
#define sharingmode_h__

#include <cstdint>

enum ESharingMode : uint32_t
{
	SHARING_MODE_EXCLUSIVE  = 0x0,
	SHARING_MODE_CONCURRENT = 0x1
};

#endif // sharingmode_h__
