#include <stdlib.h>

#include "sort_lab.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<sb::SortLab>());
	app.run();
	return EXIT_SUCCESS;
}