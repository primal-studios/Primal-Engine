#include "assets/AssetManager.h"

#include <tbb/task_scheduler_init.h>

AssetManager& AssetManager::instance()
{
	static AssetManager* instance = new AssetManager();
	return *instance;
}

void AssetManager::_loadAssetAsync(std::list<std::string>::iterator& aIter, const uint8_t aPrio)
{
	const auto asset = mAssets.find(*aIter);
	if (asset != mAssets.end())
	{
		mAsyncTaskGroup.run([=] {asset->second->load(); });
	}

	switch (aPrio)
	{
		case ASSET_LOAD_HIGH_PRIO:
			mAsyncAssetQueueHigh.erase(aIter);
			aIter = mAsyncAssetQueueHigh.begin();
			break;

		case ASSET_LOAD_MED_PRIO:
			mAsyncAssetQueueMedium.erase(aIter);
			aIter = mAsyncAssetQueueMedium.begin();
			break;

		case ASSET_LOAD_LOW_PRIO:
			mAsyncAssetQueueLow.erase(aIter);
			aIter = mAsyncAssetQueueLow.begin();
			break;

		default:
			break;
	}

	// Cramming tasks down TBB's throat causes tasks to drop
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void AssetManager::_loadAssetSync(std::list<std::string>::iterator& aIter, const uint8_t aPrio)
{
	auto asset = mAssets.find(*aIter);
	if (asset != mAssets.end())
		asset->second->load();

	switch (aPrio)
	{
		case ASSET_LOAD_HIGH_PRIO:
			mSyncAssetQueueHigh.erase(aIter);
			aIter = mSyncAssetQueueHigh.begin();
			break;

		case ASSET_LOAD_MED_PRIO:
			mSyncAssetQueueMedium.erase(aIter);
			aIter = mSyncAssetQueueMedium.begin();
			break;

		case ASSET_LOAD_LOW_PRIO:
			mSyncAssetQueueLow.erase(aIter);
			aIter = mSyncAssetQueueLow.begin();
			break;

		default:
			break;
	}
}

AssetManager::AssetManager()
{
	tbb::task_scheduler_init(4);

	// Do Async Loads //
	mAsyncTaskGroup.run([=]
	{
		while (true)
		{
			auto it = mAsyncAssetQueueHigh.begin();
			while (it != mAsyncAssetQueueHigh.end())
				_loadAssetAsync(it, ASSET_LOAD_HIGH_PRIO);

			it = mAsyncAssetQueueMedium.begin();
			while (it != mAsyncAssetQueueMedium.end())
			{
				if (mAsyncAssetQueueHigh.size() > 0)
					break;
				_loadAssetAsync(it, ASSET_LOAD_MED_PRIO);
			}

			it = mAsyncAssetQueueLow.begin();
			while (it != mAsyncAssetQueueLow.end())
			{
				if (mAsyncAssetQueueMedium.size() > 0 || mAsyncAssetQueueHigh.size() > 0)
					break;
				_loadAssetAsync(it, ASSET_LOAD_LOW_PRIO);
			}
		}
	});

	// Do Sync loads //
	mSyncTaskGroup.run([=]
	{
		while (true)
		{
			auto it = mSyncAssetQueueHigh.begin();
			while (it != mSyncAssetQueueHigh.end())
				_loadAssetSync(it, ASSET_LOAD_HIGH_PRIO);

			it = mSyncAssetQueueMedium.begin();
			while (it != mSyncAssetQueueMedium.end())
			{
				if (mSyncAssetQueueHigh.size() > 0)
					break;
				_loadAssetSync(it, ASSET_LOAD_MED_PRIO);
			}

			it = mSyncAssetQueueLow.begin();
			while (it != mSyncAssetQueueLow.end())
			{
				if (mSyncAssetQueueMedium.size() > 0 || mSyncAssetQueueHigh.size() > 0)
					break;
				_loadAssetSync(it, ASSET_LOAD_LOW_PRIO);
			}
		}
	});
}

void AssetManager::unloadAll()
{
	mAssets.clear();
}

bool AssetManager::unload(const std::string& aName)
{
	const auto loc = mAssets.find(aName);
	if (loc == mAssets.end())
		return false;

	mAssets.erase(loc);
	return true;
}