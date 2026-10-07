#include "application/application.h"
#include "sph.h"

#include <cstdlib>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<sph::SPHFluidSimulation>());
	app.run();
	return EXIT_SUCCESS;
}
