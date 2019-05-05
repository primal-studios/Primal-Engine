#ifndef application_h__
#define application_h__

#include "core/Window.h"

#include "events/Event.h"
#include "events/ApplicationEvent.h"
#include "events/KeyEvents.h"

#include "input/Input.h"

#include "application/ApplicationLayerStack.h"
#include "application/ApplicationLayer.h"

class Application
{
	public:
		Application();
		virtual ~Application();

		void run() const;

		void onEvent(Event& aEvent);

		void pushLayer(ApplicationLayer* aLayer);
		void popLayer(ApplicationLayer* aLayer);

		void pushOverlay(ApplicationLayer* aOverlay);
		void popOverlay(ApplicationLayer* aOverlay);

		Window& getWindoe() const { return *mWindow; }
		static Application& get() { return *sInstance; }

	private:
		static Application* sInstance;
		bool mRunning = true;

		Window* mWindow;

		ApplicationLayerStack mLayerStack;

		// Events
		bool _onWindowClose(WindowCloseEvent& aEvent);
		bool _onKeyPressed(KeyPressedEvent& aEvent) const;
		bool _onKeyReleased(KeyReleasedEvent& aEvent) const;
};

Application* createApplication();

#endif // application_h__