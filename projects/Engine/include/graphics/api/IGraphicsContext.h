#ifndef igraphicscontext_h__
#define igraphicscontext_h__

#include <cstdint>
#include <string>

#include <GLFW/glfw3.h>

struct GraphicsContextCreateInfo
{
	std::string applicationName;
	GLFWwindow* window;
	uint32_t versionMajor;
	uint32_t versionMinor;
	uint32_t versionPatch;
};

class IGraphicsContext
{
	public:
		explicit IGraphicsContext(const GraphicsContextCreateInfo& aCreateInfo);
		IGraphicsContext(const IGraphicsContext&) = delete;
		IGraphicsContext(IGraphicsContext&&) noexcept = delete;
		virtual ~IGraphicsContext() = default;

		IGraphicsContext& operator=(const IGraphicsContext&) = delete;
		IGraphicsContext& operator=(IGraphicsContext&&) noexcept = delete;

		virtual void idle() const = 0;
		virtual uint64_t getSurfaceHandle() const = 0;
};

#endif // igraphicscontext_h__
