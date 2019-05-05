#ifndef application_event_h__
#define application_event_h__

#include <sstream>
#include <string>
#include <cstdint>

#include "events/Event.h"

class WindowCreatedEvent final : public Event
{
	public:
		explicit WindowCreatedEvent(const std::string& aName)
			: mName(aName)
		{
			
		}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowCreatedEvent: " << mName;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowCreatedEvent)

	private:
		std::string mName;
};

class WindowResizeEvent final : public Event
{
	public:
		WindowResizeEvent(const uint32_t aWidth, const uint32_t aHeight)
			: mWidth(aWidth), mHeight(aHeight)
		{
			
		}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << mWidth << ", " << mHeight;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResizeEvent)

	private:
		uint32_t mWidth;
		uint32_t mHeight;
};

class WindowCloseEvent final : public Event
{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowCloseEvent)
};

#endif // application_event_h__