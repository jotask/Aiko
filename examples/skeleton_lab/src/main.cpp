#include <stdlib.h>

#include "skeleton_lab.h"

#include <application/application.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<lab::SkeletonLab>());
	app.run();
	return EXIT_SUCCESS;
}