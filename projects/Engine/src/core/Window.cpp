#include "core/Window.h"

#include <glad/glad.h>

#include "core/Log.h"
#include "core/PrimalAssert.h"

#include "events/ApplicationEvent.h"

static bool glfwInitialized = false;

static void sGlfwErrorCallback(const int32_t aError, const char* aDescription)
{
	PRIMAL_INTERNAL_ERROR("GLFW Error ({0}): {1}", aError, aDescription);
}

Window* Window::create(const WindowProperties& aProps)
{
	return new Window(aProps);
}

Window::Window(const WindowProperties& aProps)
{
	mData.title = aProps.title;
	mData.width = static_cast<int32_t>(aProps.width);
	mData.height = static_cast<int32_t>(aProps.height);

	mData.fullscreen = aProps.fullscreen;
	mData.vSync = aProps.vSync;

	if(!glfwInitialized)
	{
		const int32_t status = glfwInit();
		PRIMAL_INTERNAL_ASSERT(status, "Could not initialize GLFW");
		glfwSetErrorCallback(sGlfwErrorCallback);
		glfwInitialized = true;
	}

	if(mData.fullscreen)
	{
		mWindow = glfwCreateWindow(mData.width, mData.height, mData.title.c_str(), glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		mWindow = glfwCreateWindow(mData.width, mData.height, mData.title.c_str(), nullptr, nullptr);
	}

	glfwMakeContextCurrent(mWindow);

	const int32_t status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
	PRIMAL_INTERNAL_ASSERT(status, "Could not initialize Glad");

	glfwSetWindowUserPointer(mWindow, &mData);

	glfwSwapInterval(static_cast<int32_t>(mData.vSync));

	glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* aWindow, const int32_t aWidth, const int32_t aHeight)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));
		data.width = aWidth;
		data.height = aHeight;

		WindowResizeEvent event(aWidth, aHeight);
		data.eventCallback(event);
	});

	glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* aWindow)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));
		WindowCloseEvent event;
		data.eventCallback(event);
	});
}

Window::~Window()
{
	glfwDestroyWindow(mWindow);
}

void Window::pollEvents() const
{
	glfwPollEvents();
}

void Window::refresh() const
{
	glfwSwapBuffers(mWindow);
}

void Window::setVSync(const bool aEnabled)
{
	glfwSwapInterval(aEnabled);

	mData.vSync = aEnabled;
}

void Window::setFullscreen(const bool aEnabled)
{
	if (aEnabled)
	{
		glfwGetWindowPos(mWindow, &mData.xPos, &mData.yPos);
		glfwGetWindowSize(mWindow, &mData.width, &mData.height);

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwSetWindowMonitor(mWindow, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
	}
	else
	{
		glfwSetWindowMonitor(mWindow, nullptr, mData.xPos, mData.yPos, mData.width, mData.height, GLFW_DONT_CARE);
	}

	mData.fullscreen = aEnabled;
}

