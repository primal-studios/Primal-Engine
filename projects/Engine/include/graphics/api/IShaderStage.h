#ifndef ishaderstage_h__
#define ishaderstage_h__

#include <string>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IShaderModule.h"

enum EShaderStageFlags : uint32_t
{
	SHADER_STAGE_VERTEX = 0x00000001,
	SHADER_STAGE_TESSELLATION_CONTROL = 0x00000002,
	SHADER_STAGE_TESSELLATION_EVALUATION = 0x00000004,
	SHADER_STAGE_GEOMETRY = 0x00000008,
	SHADER_STAGE_FRAGMENT = 0x00000010,
	SHADER_STAGE_COMPUTE = 0x00000020,
	SHADER_STAGE_ALL_GRAPHICS = 0x0000001F
};

struct ShaderStageCreateInfo
{
	uint32_t flags;
	EShaderStageFlags stage;
	IShaderModule* module;
	std::string name;
};

class IShaderStage
{
	public:
		explicit IShaderStage(IGraphicsContext* aContext);
		IShaderStage(const IShaderStage&) = delete;
		IShaderStage(IShaderStage&&) noexcept = delete;
		virtual ~IShaderStage() = default;

		IShaderStage& operator=(const IShaderStage&) = delete;
		IShaderStage& operator=(IShaderStage&&) noexcept = delete;

		virtual void construct(const ShaderStageCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // ishaderstage_h__