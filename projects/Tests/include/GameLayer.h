#ifndef gamelayer_h__
#define gamelayer_h__

#include <application/ApplicationLayer.h>

class GameLayer : public ApplicationLayer
{
	public:
		GameLayer();
		~GameLayer();

		void onAttach() override;

		void onUpdate() override;
		void onRender() override;

		void onDetach() override;

		void onEvent(Event& aEvent) override;
};

#endif // gamelayer_h__