#ifndef window_h__
#define window_h__

#include <cstdint>
#include <string>
#include <functional>

#include <GLFW/glfw3.h>

#include "events/Event.h"

struct WindowProperties
{
	uint32_t width;
	uint32_t height;

	std::string title;

	bool vSync;
	bool fullscreen;

	explicit WindowProperties(const std::string& aTitle = "Primal Engine",
							  const uint32_t aWidth = 1280,
							  const uint32_t aHeight = 720,
							  const bool aVSync = false,
							  const bool aFullscreen = false)
		: width(aWidth), height(aHeight), title(aTitle), vSync(aVSync), fullscreen(aFullscreen)
	{
		
	}
};

class Window
{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;
		static Window* create(const WindowProperties& aProps = WindowProperties());

		explicit Window(const WindowProperties& aProps);
		~Window();

		void pollEvents() const;
		void refresh() const;

		uint32_t width() const { return mData.width; }
		uint32_t height() const { return mData.height; }

		bool isVSync() const { return mData.vSync; }
		bool isFullscreen() const { return mData.fullscreen; }

		void setEventCallback(const EventCallbackFunction& aCallback) { mData.eventCallback = aCallback; }
		void setVSync(const bool aEnabled);
		void setFullscreen(const bool aEnabled);

		void setResolution(const uint32_t aWidth, const uint32_t aHeight);

		void close() const;

		GLFWwindow* getNativeWindow() const { return mWindow; }

	private:
		GLFWwindow* mWindow;

		struct WindowData
		{
			int32_t width;
			int32_t height;

			std::string title;

			bool vSync;
			bool fullscreen;

			EventCallbackFunction eventCallback;

			int32_t xPos;
			int32_t yPos;
		};

		WindowData mData;
};

#endif // window_h__