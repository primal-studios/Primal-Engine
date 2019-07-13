#ifndef assetmanager_h__
#define assetmanager_h__

#include <utility>
#include <unordered_map>
#include <string>
#include <type_traits>
#include <memory>
#include <list>

#include <tbb/task_group.h>

#include "assets/Asset.h"
#include <typeindex>
#include <functional>

constexpr uint8_t assetLoadLowPrio = 0;
constexpr uint8_t assetLoadMedPrio = 1;
constexpr uint8_t assetLoadHighPrio = 2;

class AssetManager
{
	public:
		static AssetManager& instance();

		template<typename T, typename ... Arguments>
		std::shared_ptr<T> load(const std::string& aName,
			Arguments&& ... aArgs);

		template<typename T, typename ... Arguments>
		std::shared_ptr<T> loadAsync(const std::string& aName,
			const uint8_t aPrio,
			Arguments&& ... aArgs);

		template<typename T, typename ... Arguments>
		std::shared_ptr<T> add(const std::string& aName,
			Arguments&& ... aArgs);

		template<typename T>
		std::shared_ptr<T> get(const std::string& aName);

		bool unload(const std::string& aName);
		void unloadAll();

	private:
		void _loadAssetAsync(std::list<std::string>::iterator& aIter, const uint8_t aPrio);

		tbb::task_group mAsyncTaskGroup;

		std::list<std::string> mAsyncAssetQueueHigh;
		std::list<std::string> mAsyncAssetQueueMedium;
		std::list<std::string> mAsyncAssetQueueLow;

		AssetManager();

		std::unordered_map<std::string, std::shared_ptr<Asset>> mAssets;
};

template<typename T, typename ... Arguments>
std::shared_ptr<T> AssetManager::load(const std::string& aName,
									  Arguments&& ... aArgs)
{
	static_assert(std::is_base_of<Asset, T>::value, "T is not derived from Asset");

	if(get<T>(aName) != nullptr)
	{
		return get<T>(aName);
	}

	std::shared_ptr<T> asset = std::make_shared<T>(std::forward<Arguments>(aArgs)...);
	asset->mName = aName;

	asset->_load();

	mAssets[aName] = asset;

	return asset;
}

template <typename T, typename ... Arguments>
std::shared_ptr<T> AssetManager::loadAsync(const std::string& aName,
										   const uint8_t aPrio,
										   Arguments&&... aArgs)
{
	static_assert(std::is_base_of<Asset, T>::value, "T is not derived from Asset");

	std::shared_ptr<T> asset = std::make_shared<T>(std::forward<Arguments>(aArgs)...);
	asset->mName = aName;

	switch (aPrio)
	{
		case assetLoadHighPrio:
			mAsyncAssetQueueHigh.push_back(aName);
			break;

		case assetLoadMedPrio:
			mAsyncAssetQueueMedium.push_back(aName);
			break;

		case assetLoadLowPrio:
			mAsyncAssetQueueLow.push_back(aName);
			break;

		default:
			break;
	}

	mAssets[aName] = asset;

	return asset;
}

template<typename T, typename ... Arguments>
std::shared_ptr<T> AssetManager::add(const std::string& aName,
	Arguments&& ... aArgs)
{
	static_assert(std::is_base_of<Asset, T>::value, "T is not derived from Asset");

	std::shared_ptr<T> asset = std::make_shared<T>(std::forward<Arguments>(aArgs)...);
	asset->mName = aName;
	asset->mLoaded = true;

	mAssets[aName] = asset;

	return asset;
}

template<typename T>
std::shared_ptr<T> AssetManager::get(const std::string& aName)
{
	static_assert(std::is_base_of<Asset, T>::value, "T is not derived from Asset");

	const auto element = mAssets.find(aName);
	if (element != mAssets.end())
		return std::static_pointer_cast<T>(element->second);

	return nullptr;
}

#endif // assetmanager_h__
