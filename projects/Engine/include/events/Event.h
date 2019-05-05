#ifndef event_h__
#define event_h__

#include <functional>
#include <ostream>
#include <string>

#define BIND_EVENT_FUNCTION(x) std::bind(&x, this, std::placeholders::_1)

#define EVENT_CLASS_TYPE(type) static size_t getStaticType() { return typeid(type).hash_code(); }\
								virtual size_t getEventType() const override { return getStaticType(); }\
								virtual std::string getName() const override { return typeid(type).name(); }

class Event
{
public:
	virtual ~Event() = default;

	virtual size_t getEventType() const = 0;
	virtual std::string getName() const = 0;
	virtual std::string toString() const { return getName(); }

	bool isHandled() const
	{
		return mHandled;
	}

private:
	friend class EventDispatcher;
	bool mHandled = false;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.toString().c_str();
}

class EventDispatcher
{
	template<typename T>
	using EventFunction = std::function<bool(T&)>;

public:
	explicit EventDispatcher(Event& aEvent) : mEvent(aEvent) {}

	template<typename T>
	bool dispatch(EventFunction<T> aFunction)
	{
		if (mEvent.getEventType() == T::getStaticType())
		{
			mEvent.mHandled = aFunction(*static_cast<T*>(&mEvent));
			return true;
		}

		return false;
	}

private:
	Event& mEvent;
};

#endif // event_h__