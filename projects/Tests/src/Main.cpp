#include <Primal.h>

#include <application/EntryPoint.h>

#include "GameLayer.h"

class Sandbox final : public Application
{
	public:
		Sandbox()
		{
			pushLayer(new GameLayer());
		}

		~Sandbox()
		{
			
		}
};

Application* createApplication()
{
	return new Sandbox();
}
