#ifndef imageaspect_h__
#define imageaspect_h__

#include <cstdint>

enum EImageAspectFlagBits : uint32_t
{
	IMAGE_ASPECT_COLOR = 0x00000001,
	IMAGE_ASPECT_DEPTH = 0x00000002,
	IMAGE_ASPECT_STENCIL = 0x00000004,
	IMAGE_ASPECT_METADATA = 0x00000008,
	IMAGE_ASPECT_PLANE_0 = 0x00000010,
	IMAGE_ASPECT_PLANE_1 = 0x00000020,
	IMAGE_ASPECT_PLANE_2 = 0x00000040
};

using ImageAspectFlags = uint32_t;

#endif // imageaspect_h__
