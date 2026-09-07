#include <aiko.h>

#include <models/game_object.h>

#include <models/camera.h>
#include <models/material.h>
#include <models/mesh.h>
#include <models/model.h>
#include <models/texture.h>
#include <models/shader.h>
#include <models/compute_buffer.h>
#include <models/compute_shader.h>
#include <models/frame_buffer.h>
#include <models/render_target.h>

#include <aiko_body_desc.h>
#include <aiko_shape_desc.h>
#include <aiko_physics_types.h>
#include <aiko_query_types.h>
#include <player_controller_types.h>

#include <aiko_physics.h>
#include <aiko_player_controller.h>

#include <stdlib.h>

int main()
{
    aiko::GameObject object;

    auto& transform = object.transform();
    transform.position = {0.0f, 0.0f, 0.0f};

    aiko::Camera camera;
    aiko::Material material;
    aiko::Mesh mesh;
    aiko::Model model;
    aiko::Texture texture;
    aiko::Shader shader;
    aiko::ComputeBuffer computeBuffer;
    aiko::ComputeShader computeShader;
    aiko::FrameBuffer framebuffer;
    aiko::RenderTarget renderTarget;

    aiko::physics::AikoPhysics physics;
    aiko::physics::PlayerController playerController;

    return EXIT_SUCCESS;
}