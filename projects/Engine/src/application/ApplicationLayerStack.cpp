#include "application/ApplicationLayerStack.h"

#include <algorithm>

#include "application/ApplicationLayer.h"

ApplicationLayerStack::~ApplicationLayerStack()
{
	for(ApplicationLayer* layer : mLayers)
	{
		delete layer;
	}

	mLayers.clear();
}

void ApplicationLayerStack::pushLayer(ApplicationLayer* aLayer)
{
	mLayers.emplace(mLayers.begin() + mLayerInsert, aLayer);
	mLayerInsert++;
}

void ApplicationLayerStack::popLayer(ApplicationLayer* aLayer)
{
	const auto it = std::find(mLayers.begin(), mLayers.end(), aLayer);
	if(it != mLayers.end())
	{
		mLayers.erase(it);
		--mLayerInsert;
	}
}

void ApplicationLayerStack::pushOverlay(ApplicationLayer* aOverlay)
{
	mLayers.emplace_back(aOverlay);
}

void ApplicationLayerStack::popOverlay(ApplicationLayer* aOverlay)
{
	const auto it = std::find(mLayers.begin(), mLayers.end(), aOverlay);
	if (it != mLayers.end())
	{
		mLayers.erase(it);
	}
}

