#ifndef gamelayer_h__
#define gamelayer_h__

#include <Primal.h>

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

	private:
};

#endif // gamelayer_h__