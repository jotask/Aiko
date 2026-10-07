#include <stdlib.h>

#include <application/application.h>

#include "vulkan_shader_toy.h"

int main()
{
	aiko::Application app;
	app.pushLayer(std::make_unique<shadertoy::VulkanShaderToy>());
	app.run();
	return EXIT_SUCCESS;
}