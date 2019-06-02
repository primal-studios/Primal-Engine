#ifndef ishadermodule_h__
#define ishadermodule_h__

#include <vector>

#include "graphics/api/IGraphicsContext.h"

struct ShaderModuleCreateInfo
{
	uint32_t flags;
	std::vector<char> code;
};

class IShaderModule
{
	public:
		explicit IShaderModule(IGraphicsContext* aContext);
		IShaderModule(const IShaderModule&) = delete;
		IShaderModule(IShaderModule&&) noexcept = delete;
		virtual ~IShaderModule() = default;

		IShaderModule& operator=(const IShaderModule&) = delete;
		IShaderModule& operator=(IShaderModule&&) noexcept = delete;

		virtual void construct(const ShaderModuleCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif //ishadermodule_h__