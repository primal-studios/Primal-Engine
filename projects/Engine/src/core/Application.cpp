#include "core/Application.h"

#include "core/Log.h"

Application* Application::sInstance;

Application::Application()
{
	Log::construct();

	sInstance = this;

	mWindow = Window::create();
	mWindow->setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));
}

Application::~Application()
{
	delete mWindow;
	sInstance = nullptr;
}

void Application::run()
{
	while(mRunning)
	{
		mWindow->refresh();
		mWindow->pollEvents();
	}
}

void Application::onEvent(Event& aEvent)
{
	EventDispatcher dispatcher(aEvent);
	dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::_onWindowClose));
}

bool Application::_onWindowClose(WindowCloseEvent& aEvent)
{
	mRunning = false;
	return true;
}