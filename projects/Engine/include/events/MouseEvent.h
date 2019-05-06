#ifndef mouseevent_h__
#define mouseevent_h__

#include <string>
#include <sstream>

#include "Event.h"

class MouseMovedEvent final : public Event
{
	public:
		MouseMovedEvent(const float aX, const float aY) : mMouseX(aX), mMouseY(aY)
		{
		}

		float getX() const { return mMouseX; }
		float getY() const { return mMouseY; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseMovedEvent)

	private:
		float mMouseX;
		float mMouseY;
};

class MouseScrolledEvent final : public Event
{
	public:
		MouseScrolledEvent(const float aXOffset, const float aYOffset) : mXOffset(aXOffset), mYOffset(aYOffset)
		{
		}

		float getXOffset() const { return mXOffset; }
		float getYOffset() const { return mYOffset; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << getXOffset() << ", " << getYOffset();
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseScrolledEvent)

	private:
		float mXOffset;
		float mYOffset;
};

class MouseButtonEvent : public Event
{
	public:
		int getMouseButton() const { return mButton; }

	protected:
		explicit MouseButtonEvent(const int aButton)
			: mButton(aButton) {}

		int mButton;
};

class MouseButtonPressedEvent final : public MouseButtonEvent
{
	public:
		explicit MouseButtonPressedEvent(const int aButton)
			: MouseButtonEvent(aButton) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << mButton;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressedEvent)
};

class MouseButtonReleasedEvent final : public MouseButtonEvent
{
	public:
		explicit MouseButtonReleasedEvent(const int aButton)
			: MouseButtonEvent(aButton) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << mButton;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleasedEvent)
};

#endif // #define mouseevent_h__
