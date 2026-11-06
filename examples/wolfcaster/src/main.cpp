#include "wolfcaster.h"

#include <application/application.h>

#include <stdlib.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<wc::WolfCaster>());
	app.run();
	return EXIT_SUCCESS;
}