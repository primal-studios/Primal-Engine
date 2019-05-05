#include <Primal.h>

class Sandbox final : public Application
{
	public:
		Sandbox()
		{
			
		}

		~Sandbox()
		{
			
		}
};

Application* createApplication()
{
	return new Sandbox();
}
