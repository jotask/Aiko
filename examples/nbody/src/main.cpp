#include "nbody.h"

#include <application/application.h>

#include <stdlib.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<nbody::NBody>());
	app.run();
	return EXIT_SUCCESS;
}