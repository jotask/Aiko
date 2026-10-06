#include <stdlib.h>

#include "application/application.h"

#include "loop_recorder_gui.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<aiko::recorder::LoopRecorderGui>());
	app.run();
	return EXIT_SUCCESS;
}