#ifndef shaderstageflags_h__
#define shaderstageflags_h__

#include <cstdint>

enum EShaderStageFlagBits : uint32_t
{
	SHADER_STAGE_VERTEX = 0x00000001,
	SHADER_STAGE_TESSELLATION_CONTROL = 0x00000002,
	SHADER_STAGE_TESSELLATION_EVALUATION = 0x00000004,
	SHADER_STAGE_GEOMETRY = 0x00000008,
	SHADER_STAGE_FRAGMENT = 0x00000010,
	SHADER_STAGE_COMPUTE = 0x00000020,
	SHADER_STAGE_ALL_GRAPHICS = 0x0000001F
};

using ShaderStageFlags = uint32_t;

#endif // shaderstageflags_h__