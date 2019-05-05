#ifndef applicationlayerstack_h__
#define applicationlayerstack_h__

#include <vector>

#include "events/Event.h"

class ApplicationLayer;
class ApplicationLayerStack
{
	public:
		ApplicationLayerStack() = default;
		~ApplicationLayerStack();

		void pushLayer(ApplicationLayer* aLayer);
		void popLayer(ApplicationLayer* aLayer);

		void pushOverlay(ApplicationLayer* aOverlay);
		void popOverlay(ApplicationLayer* aOverlay);

		std::vector<ApplicationLayer*>::iterator begin() { return mLayers.begin(); }
		std::vector<ApplicationLayer*>::iterator end() { return mLayers.end(); }

		std::vector<ApplicationLayer*>::const_iterator begin() const { return mLayers.begin(); }
		std::vector<ApplicationLayer*>::const_iterator end() const { return mLayers.end(); }

	private:
		std::vector<ApplicationLayer*> mLayers;
		uint32_t mLayerInsert = 0;
};

#endif // applicationlayerstack_h__