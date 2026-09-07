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
#include <aiko_renderer.h>
#include <modules/render_module.h>
#include <modules/module.h>
#include <models/component.h>
#include <components/transform_component.h>
#include <application/application.h>
#include <layers/layer.h>
#include <scene/scene.h>
#include <systems/system.h>
#include <assets/types/mesh_asset.h>
#include <types/compute_pass.h>

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

    aiko::AikoRenderer* renderer = nullptr;
    aiko::RenderModule* renderModule = nullptr;
    aiko::Module* module = nullptr;

    (void)renderer;
    (void)renderModule;
    (void)module;

    aiko::Component* component = nullptr;
    aiko::TransformComponent transformComponent;

    (void)component;
    (void)transformComponent;

    aiko::Application* application = nullptr;
    aiko::Layer* layer = nullptr;
    aiko::System* system = nullptr;

    aiko::Scene scene;

    (void)application;
    (void)layer;
    (void)system;
    (void)scene;

    aiko::MeshAsset meshAsset;
    aiko::ModelAsset modelAsset;
    aiko::ComputeBufferDesc computeBufferDesc{};

    (void)meshAsset;
    (void)modelAsset;
    (void)computeBufferDesc;

    return EXIT_SUCCESS;
}