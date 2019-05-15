#ifndef dependencyflags_h__
#define dependencyflags_h__

#include <cstdint>

using DependencyFlags = uint32_t;

enum EDependencyFlagBits : uint32_t
{
	DEPENDENCY_BY_REGION		= 0x01,
	DEPENDENCY_DEVICE_GROUP		= 0x02,
	DEPENDENCY_LOCAL			= 0x04
};

#endif // dependencyflags_h__
