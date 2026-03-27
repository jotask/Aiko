#include <stdlib.h>

#include "life_particle.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<lp::LifeParticles>());
	app.run();
	return EXIT_SUCCESS;
}