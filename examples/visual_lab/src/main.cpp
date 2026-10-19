#include "application/application.h"
#include "visual_lab.h"

#include <cstdlib>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<aiko::lab::VisualLab>());
	app.run();
	return EXIT_SUCCESS;
}
