#include <stdlib.h>

#include "sandbox.h"
#include "application/application.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<sb::Sandbox>());
	app.run();
	return EXIT_SUCCESS;
}