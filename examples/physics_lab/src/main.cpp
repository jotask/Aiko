#include "application/application.h"
#include "physics_lab.h"

#include <cstdlib>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<aiko::lab::PhysicsLab>());
	app.run();
	return EXIT_SUCCESS;
}
