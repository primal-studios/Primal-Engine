#ifndef applicationlayer_h__
#define applicationlayer_h__

#include <string>

#include "events/Event.h"

class ApplicationLayer
{
	public:
		explicit ApplicationLayer(const std::string& aDebugName = "ApplicationLayer") 
			: mDebugName(aDebugName)
		{
			
		}

		virtual ~ApplicationLayer() = default;

		virtual void onAttach() {}
		virtual void onDetach() {}

		virtual void onUpdate() {}
		virtual void onRender() {}

		virtual void onEvent(Event& aEvent) {}

		const std::string& name() const { return mDebugName; }

	private:
		std::string mDebugName;
};

#endif // applicationlayer_h__