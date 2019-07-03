#ifndef ishaderstage_h__
#define ishaderstage_h__

#include <string>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IShaderModule.h"
#include "graphics/ShaderStageFlags.h"

struct ShaderStageCreateInfo
{
	uint32_t flags;
	EShaderStageFlagBits stage;
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
		virtual IShaderModule* getModule() const = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // ishaderstage_h__