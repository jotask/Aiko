#include <aiko.h>

#include <models/game_object.h>
#include <models/camera.h>
#include <models/material.h>
#include <models/mesh.h>
#include <models/texture.h>
#include <models/compute_buffer.h>
#include <models/frame_buffer.h>

#include <stdlib.h>

int main()
{
    aiko::GameObject object;

    auto& transform = object.transform();
    transform.position = {0.0f, 0.0f, 0.0f};

    aiko::Camera camera;
    aiko::Material material;
    aiko::Mesh mesh;
    aiko::Texture texture;
    aiko::ComputeBuffer computeBuffer;
    aiko::FrameBuffer framebuffer;

    return EXIT_SUCCESS;
}