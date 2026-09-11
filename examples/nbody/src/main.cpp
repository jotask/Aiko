#include <stdlib.h>

#include "nbody.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<nbody::NBody>());
	app.run();
	return EXIT_SUCCESS;
}