#include <stdlib.h>

#include "render_regression.h"

#include <application/application.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<regression::RenderRegression>());
	app.run();
	return EXIT_SUCCESS;
}