#include "apexborn.h"

#include <application/application.h>

#include <stdlib.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<ab::ApexBorn>());
	app.run();
	return EXIT_SUCCESS;
}