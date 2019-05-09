#include "core/Window.h"

#include "core/Log.h"
#include "core/PrimalAssert.h"

#if defined(PRIMAL_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#else
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <GLFW/glfw3native.h>

#include "events/KeyEvents.h"
#include "events/ApplicationEvent.h"
#include "events/MouseEvent.h"

static bool sGlfwInitialized = false;

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

	if(!sGlfwInitialized)
	{
		const int32_t status = glfwInit();
		PRIMAL_INTERNAL_ASSERT(status, "Could not initialize GLFW");
		glfwSetErrorCallback(sGlfwErrorCallback);
		sGlfwInitialized = true;
	}

	if(mData.fullscreen)
	{
		mWindow = glfwCreateWindow(mData.width, mData.height, mData.title.c_str(), glfwGetPrimaryMonitor(), nullptr);
	}
	else
	{
		mWindow = glfwCreateWindow(mData.width, mData.height, mData.title.c_str(), nullptr, nullptr);
	}

	//glfwMakeContextCurrent(mWindow);

	setVSync(mData.vSync);

	glfwSetWindowUserPointer(mWindow, &mData);

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

	glfwSetKeyCallback(mWindow, [](GLFWwindow* aWindow, const int32_t aKey, const int32_t aScanCode, const int32_t aAction, const int32_t aMods)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));

		switch (aAction)
		{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(aKey, 0);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(aKey);
				data.eventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(aKey, 1);
				data.eventCallback(event);
				break;
			}

			default:
				break;
		}
	});

	glfwSetCharCallback(mWindow, [](GLFWwindow* aWindow, const uint32_t aKeyCode)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));

		KeyTypedEvent event(aKeyCode);
		data.eventCallback(event);
	});

	glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* aWindow, const int32_t aButton, const int32_t aAction, const int32_t aMods)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));

		switch (aAction)
		{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(aButton);
				data.eventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(aButton);
				data.eventCallback(event);
				break;
			}

			default:
				break;
		}
	});

	glfwSetScrollCallback(mWindow, [](GLFWwindow* aWindow, const double aXOffset, const double aYOffset)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));

		MouseScrolledEvent event(static_cast<float>(aXOffset), static_cast<float>(aYOffset));
		data.eventCallback(event);
	});

	glfwSetCursorPosCallback(mWindow, [](GLFWwindow* aWindow, const double aXPos, const double aYPos)
	{
		WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(aWindow));

		MouseMovedEvent event(static_cast<float>(aXPos), static_cast<float>(aYPos));
		data.eventCallback(event);
	});
}

Window::~Window()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
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
//	glfwSwapInterval(aEnabled);

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

void Window::setResolution(const uint32_t aWidth, const uint32_t aHeight)
{
	mData.width = static_cast<int32_t>(aWidth);
	mData.height = static_cast<int32_t>(aHeight);

	glfwSetWindowSize(mWindow, mData.width, mData.height);
}

void Window::close() const
{
	glfwSetWindowShouldClose(mWindow, true);

	WindowCloseEvent event;
	mData.eventCallback(event);
}

