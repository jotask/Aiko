#include <stdlib.h>

#include "shader_toy.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<shadertoy::ShaderToy>());
	app.run();
	return EXIT_SUCCESS;
}