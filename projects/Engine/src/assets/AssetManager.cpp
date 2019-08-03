#include "assets/AssetManager.h"

#include <tbb/task_scheduler_init.h>
#include <thread>

AssetManager& AssetManager::instance()
{
	static AssetManager* instance = new AssetManager();
	return *instance;
}

// NOT THREAD SAFE
void AssetManager::_loadAssetAsync(std::list<std::string>::iterator& aIter, const uint8_t aPrio)
{
	const auto asset = mAssets.find(*aIter);
	if (asset != mAssets.end())
	{
		mAsyncTaskGroup.run([=] 
		{
			asset->second->_load();
		});
	}

	switch (aPrio)
	{
		case assetLoadHighPrio:
			mAsyncAssetQueueHigh.erase(aIter);
			aIter = mAsyncAssetQueueHigh.begin();
			break;

		case assetLoadMedPrio:
			mAsyncAssetQueueMedium.erase(aIter);
			aIter = mAsyncAssetQueueMedium.begin();
			break;

		case assetLoadLowPrio:
			mAsyncAssetQueueLow.erase(aIter);
			aIter = mAsyncAssetQueueLow.begin();
			break;

		default:
			break;
	}

	// Cramming tasks down TBB's throat causes tasks to drop
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

AssetManager::AssetManager()
{
	tbb::task_scheduler_init(4);

	// Do Async Loads //
	mAsyncTaskGroup.run([=]
	{
		while (true)
		{
			mQueueMutex.lock();
			auto it = mAsyncAssetQueueHigh.begin();
			while (it != mAsyncAssetQueueHigh.end())
			{
				_loadAssetAsync(it, assetLoadHighPrio);
			}
			mQueueMutex.unlock();

			mQueueMutex.lock();
			it = mAsyncAssetQueueMedium.begin();
			while (it != mAsyncAssetQueueMedium.end())
			{
				if (mAsyncAssetQueueHigh.size() > 0)
				{
					mQueueMutex.unlock();
					break;
				}
				_loadAssetAsync(it, assetLoadMedPrio);
			}
			mQueueMutex.unlock();

			mQueueMutex.lock();
			it = mAsyncAssetQueueLow.begin();
			while (it != mAsyncAssetQueueLow.end())
			{
				if (mAsyncAssetQueueMedium.size() > 0 || mAsyncAssetQueueHigh.size() > 0)
				{
					mQueueMutex.unlock();
					break;
				}
				_loadAssetAsync(it, assetLoadLowPrio);
			}
			mQueueMutex.unlock();

			std::unique_lock<std::mutex> lock(mCvMutex);
			while (!mWaitingAssets)
			{
				mCv.wait(lock);
			}
			--mWaitingAssets;
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