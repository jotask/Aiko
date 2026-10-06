#include "vulkan_shader_toy.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"

namespace shadertoy
{

    void VulkanShaderToy::init()
    {
        scene().createCamera(aiko::camera::CameraController::Static);

        m_material.m_shaderId = assets().loadShader("shadertoy/shadertoy_fullscreen", "shadertoy/shadertoy_image_test" );

        m_material.m_renderState.depthTest = false;
        m_material.m_renderState.depthWrite = false;

        m_material.setVec3("iResolution", 1.0f, 1.0f, 1.0f);
        m_material.setFloat("iTime", 0.0f);

    }

    void VulkanShaderToy::update()
    {
        const float deltaTime = getDeltaTime();

        m_time += deltaTime;

        const aiko::ivec2 renderSize = renderer().getRenderSize();

        m_material.setVec3("iResolution", static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 1.0f);

        m_material.setFloat("iTime", m_time);
    }

    void VulkanShaderToy::render()
    {
        renderer().drawFullscreen(m_material);
    }

}