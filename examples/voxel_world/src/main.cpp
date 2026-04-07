#include <stdlib.h>

#include "voxel_world.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<vw::VoxelWorld>());
	app.run();
	return EXIT_SUCCESS;
}