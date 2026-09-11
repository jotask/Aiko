#include <stdlib.h>

#include "cellular_automaton.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<aiko::ca::CellularAutomaton>());
	app.run();
	return EXIT_SUCCESS;
}