#ifndef entrypoint_h__
#define entrypoint_h__

#include "application/Application.h"

extern Application* createApplication();

int main(int aArgc, char** aArgv)
{
	const auto app = createApplication();
	app->run();

	delete app;
}

#endif // entrypoint_h__