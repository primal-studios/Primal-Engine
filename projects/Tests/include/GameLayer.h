#ifndef gamelayer_h__
#define gamelayer_h__

#include <application/ApplicationLayer.h>

#include <graphics/VertexArray.h>
#include <graphics/VertexBuffer.h>
#include <graphics/IndexBuffer.h>

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
		VertexArray* vao;
};

#endif // gamelayer_h__