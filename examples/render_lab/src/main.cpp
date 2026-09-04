#include "application/application.h"
#include "render_lab.h"

#include <cstdlib>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<aiko::lab::RenderLab>());
	app.run();
	return EXIT_SUCCESS;
}
