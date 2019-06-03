#ifndef systemmanager_h__
#define systemmanager_h__

#include <list>

#include "ecs/System.h"

class SystemManager
{
	public:
		static SystemManager& instance();

		void configure();

		template<typename T, typename ... Arguments>
		T* addSystem(Arguments && ... aArgs);

		template<typename T>
		T* getSystem();

		template<typename T>
		bool hasSystem();

		template<typename T>
		void removeSystem();

		void update();
		void fixedUpdate();

		void render();

		void dispatchEvent(Event& aEvent);

	private:
		SystemManager() = default;

		std::list<System*> mSystems;
};

template <typename T, typename ... Arguments>
T* SystemManager::addSystem(Arguments&&... aArgs)
{
	static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

	T* system = new T(std::forward<Arguments>(aArgs)...);
	mSystems.push_back(system);

	return system;
}

template <typename T>
T* SystemManager::getSystem()
{
	static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

	for (const auto& system : mSystems)
	{
		if (auto check = dynamic_cast<T*>(system))
		{
			return check;
		}
	}

	return nullptr;
}

template <typename T>
bool SystemManager::hasSystem()
{
	static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

	for (const auto& system : mSystems)
	{
		if (auto check = dynamic_cast<T*>(system))
		{
			return true;
		}
	}

	return false;
}

template <typename T>
void SystemManager::removeSystem()
{
	static_assert(std::is_base_of<System, T>::value, "T is not derived from System");

	for (const auto& system : mSystems)
	{
		if (auto check = dynamic_cast<T*>(system))
		{
			auto iter = std::find(mSystems.begin(), mSystems.end(), check);
			delete *iter;
			mSystems.erase(iter);
			
			break;
		}
	}
}

#endif // systemmanager_h__
