#include "native_shader_toy.h"

#include <stdlib.h>

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<shadertoy::NativeShaderToy>());
	app.run();
	return EXIT_SUCCESS;
}