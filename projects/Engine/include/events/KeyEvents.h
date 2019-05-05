#ifndef keyevents_h__
#define keyevents_h__

#include <cstdint>
#include <string>
#include <sstream>

#include "events/Event.h"

class KeyEvent : public Event
{
	public:
		inline uint32_t getKeyCode() const { return mKeyCode; }

	protected:
		explicit KeyEvent(const uint32_t aKeyCode) : mKeyCode(aKeyCode)
		{
		}

		uint32_t mKeyCode;
};

class KeyPressedEvent final : public KeyEvent
{
	public:
		KeyPressedEvent(const uint32_t aKeyCode, const uint32_t aRepeatCount) : KeyEvent(aKeyCode), mRepeatCount(aRepeatCount)
		{
		}

		inline uint32_t getRepeatCount() const { return mRepeatCount; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeats)";
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(KeyPressedEvent)

	private:
		uint32_t mRepeatCount;
};

class KeyReleasedEvent final : public KeyEvent
{
	public:
		explicit KeyReleasedEvent(const uint32_t aKeyCode) : KeyEvent(aKeyCode)
		{
		}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(KeyReleasedEvent)
};

class KeyTypedEvent final : public KeyEvent
{
	public:
		explicit KeyTypedEvent(const uint32_t aKeyCode) : KeyEvent(aKeyCode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << mKeyCode;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(KeyTypedEvent)
};

#endif // keyevents_h__