#ifndef clearvalue_h__
#define clearvalue_h__

#include <cstdint>

union ClearColorValue
{
	float float32[4];
	int32_t int32[4];
	uint32_t uint32[4];
};

struct ClearDepthStencilValue
{
	float depth;
	uint32_t stencil;
};

union ClearValue
{
	ClearColorValue color;
	ClearDepthStencilValue depthStencil;
};

#endif // clearvalue_h__
