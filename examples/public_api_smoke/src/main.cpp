// Core
#include <aiko_types.h>

#include <assets/asset_id.h>
#include <assets/asset_reference.h>
#include <assets/asset_traits.h>
#include <assets/asset_type.h>

#include <assets/types/material_asset.h>
#include <assets/types/mesh_asset.h>
#include <assets/types/shader_asset.h>
#include <assets/types/texture_asset.h>

#include <core/transform.h>
#include <core/uuid.h>

#include <events/event.hpp>

#include <types/color.h>

// Renderer facade
#include <aiko_renderer.h>

// Renderer runtime resources
#include <models/camera.h>
#include <models/light.h>
#include <models/material.h>
#include <models/mesh.h>
#include <models/model.h>
#include <models/texture.h>
#include <models/shader.h>
#include <models/compute_buffer.h>
#include <models/compute_shader.h>
#include <models/frame_buffer.h>
#include <models/render_target.h>

#include <resources/render_resource_manager.h>

// Renderer public metadata
#include <metadata/material_instance.h>
#include <metadata/texture_meta.h>

// Renderer public types
#include <types/aiko_renderer_types.h>
#include <types/compute_pass.h>
#include <types/draw_types.h>
#include <types/render_config.h>
#include <types/render_state.h>
#include <types/render_types.h>
#include <types/texture_types.h>
#include <types/transient_types.h>
#include <types/uniform_value.h>

// Engine facade
#include <aiko.h>
#include <aiko_config.h>
#include <aiko_includes.h>

#include <application/application.h>

#include <layers/layer.h>

// Engine object/component API
#include <models/component.h>
#include <models/game_object.h>

#include <components/transform_component.h>
#include <components/camera_component.h>
#include <components/light_component.h>
#include <components/mesh_component.h>
#include <components/model_component.h>
#include <components/sprite_component.h>
#include <components/compute_shader_component.h>
#include <components/particle_emitter_component.h>
#include <components/player_controller_component.h>
#include <components/rigid_body_component.h>
#include <components/render_component_types.h>

// Engine scene/system/module API
#include <scene/scene.h>

#include <systems/system.h>

#include <modules/module.h>
#include <modules/base_module.h>
#include <modules/render_module.h>

// Physics
#include <aiko_physics.h>
#include <aiko_player_controller.h>

#include <aiko_body_desc.h>
#include <aiko_shape_desc.h>
#include <aiko_physics_types.h>
#include <aiko_query_types.h>
#include <player_controller_types.h>

#include <layers/contexts/job_context.h>
#include <types/job_types.h>

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

    AIKO_UNUSED(renderer);
    AIKO_UNUSED(renderModule);
    AIKO_UNUSED(module);

    aiko::Component* component = nullptr;
    aiko::TransformComponent transformComponent;

    AIKO_UNUSED(component);
    AIKO_UNUSED(transformComponent);

    aiko::Application* application = nullptr;
    aiko::Layer* layer = nullptr;
    aiko::System* system = nullptr;

    aiko::Scene scene;

    AIKO_UNUSED(application);
    AIKO_UNUSED(layer);
    AIKO_UNUSED(system);
    AIKO_UNUSED(scene);

    aiko::AikoConfig config{};

    aiko::MaterialAsset materialAsset{};
    aiko::ShaderAsset shaderAsset{};
    aiko::TextureAsset textureAsset;
    aiko::MeshAsset meshAsset;
    aiko::ModelAsset modelAsset;

    aiko::RendererConfig rendererConfig{};
    aiko::RenderSurfaceDesc surfaceDesc{};
    aiko::RenderState renderState{};
    aiko::ComputeBufferDesc computeBufferDesc{};

    AIKO_UNUSED(config);
    AIKO_UNUSED(materialAsset);
    AIKO_UNUSED(shaderAsset);
    AIKO_UNUSED(textureAsset);
    AIKO_UNUSED(meshAsset);
    AIKO_UNUSED(modelAsset);
    AIKO_UNUSED(rendererConfig);
    AIKO_UNUSED(surfaceDesc);
    AIKO_UNUSED(renderState);
    AIKO_UNUSED(computeBufferDesc);

    aiko::JobStats jobStats{};
    AIKO_UNUSED(jobStats);

    return EXIT_SUCCESS;
}