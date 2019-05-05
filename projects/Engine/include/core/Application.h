#ifndef application_h__
#define application_h__

#include "core/Window.h"

#include "events/Event.h"
#include "events/ApplicationEvent.h"

class Application
{
	public:
		Application();
		virtual ~Application();

		void run();

		void onEvent(Event& aEvent);

		Window& getWindoe() const { return *mWindow; }
		static Application& get() { return *sInstance; }

	private:
		static Application* sInstance;
		bool mRunning = true;

		Window* mWindow;

		bool _onWindowClose(WindowCloseEvent& aEvent);
};

Application* createApplication();

#endif // application_h__