#include "application/application.h"
#include "planet_earth.h"

#include <stdlib.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<pe::PlanetEarth>());
	app.run();
	return EXIT_SUCCESS;
}