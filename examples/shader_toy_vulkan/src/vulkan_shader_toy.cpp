#include "vulkan_shader_toy.h"

#include "layers/contexts/asset_context.h"
#include "layers/contexts/render_context.h"
#include "layers/contexts/scene_context.h"
#include "layers/contexts/input_context.h"

#include <chrono>
#include <cmath>
#include <ctime>

namespace shadertoy
{

    namespace
    {

        aiko::vec4 getShaderToyDate()
        {
            using Clock = std::chrono::system_clock;

            const auto now = Clock::now();
            const std::time_t time = Clock::to_time_t(now);

            std::tm localTime{};

            #if defined(AIKO_WINDOWS)
            localtime_s(&localTime, &time);
            #else
            localtime_r(&time, &localTime);
            #endif

            const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( now.time_since_epoch() ).count() % 1000;

            const float secondsSinceMidnight =
                static_cast<float>(localTime.tm_hour * 60 * 60) +
                static_cast<float>(localTime.tm_min * 60) +
                static_cast<float>(localTime.tm_sec) +
                static_cast<float>(milliseconds) / 1000.0f;

            return
            {
                static_cast<float>(localTime.tm_year + 1900),
                static_cast<float>(localTime.tm_mon),
                static_cast<float>(localTime.tm_mday),
                secondsSinceMidnight
            };
        }

    }

    void VulkanShaderToy::init()
    {
        scene().createCamera(aiko::camera::CameraController::Static);

        m_material.m_shaderId = assets().loadShader("shadertoy/shadertoy_fullscreen", "shadertoy/shadertoy_image_test" );

        m_material.m_renderState.depthTest = false;
        m_material.m_renderState.depthWrite = false;

        m_material.setVec3("iResolution", 1.0f, 1.0f, 1.0f);
        m_material.setFloat("iTime", 0.0f);
        m_material.setFloat("iTimeDelta", 0.0f);
        m_material.setFloat("iFrameRate", 0.0f);
        m_material.setInt("iFrame", 0);

        m_material.setVec4("iMouse", 0.0f, 0.0f, 0.0f, 0.0f);
        m_material.setVec4("iDate", getShaderToyDate());

    }

    void VulkanShaderToy::update()
    {
        const float deltaTime = getDeltaTime();

        m_time += deltaTime;

        const aiko::ivec2 renderSize = renderer().getRenderSize();

        m_material.setVec3(
            "iResolution",
            static_cast<float>(renderSize.x),
            static_cast<float>(renderSize.y),
            1.0f
        );

        m_material.setFloat("iTime", m_time);
        m_material.setFloat("iTimeDelta", deltaTime);

        const float frameRate = deltaTime > 0.0f ? 1.0f / deltaTime : 0.0f;

        m_material.setFloat("iFrameRate", frameRate);
        m_material.setInt("iFrame", m_frame);

        // --------------------------------------------------
        // Mouse
        // --------------------------------------------------

        aiko::vec2 mousePosition = input().getMouseFramebufferPosition();

        mousePosition.y = static_cast<float>(renderSize.y) - mousePosition.y;

        constexpr aiko::MouseButton LeftButton = aiko::MouseButton::MOUSE_BUTTON_LEFT;

        const bool pressed = input().isMouseButtonPressed(LeftButton);

        const bool justPressed = input().isMouseButtonJustPressed(LeftButton);

        if (justPressed)
        {
            m_mousePosition = mousePosition;
            m_mouseClickPosition = mousePosition;
            m_hasMouseClick = true;
        }
        else if (pressed)
        {
            m_mousePosition = mousePosition;
        }

        aiko::vec4 shaderToyMouse{};

        if (m_hasMouseClick)
        {
            const float clickX = std::abs(m_mouseClickPosition.x);
            const float clickY = std::abs(m_mouseClickPosition.y);

            if (pressed)
            {
                shaderToyMouse =
                {
                    m_mousePosition.x,
                    m_mousePosition.y,
                    clickX,
                    justPressed ? clickY : -clickY
                };
            }
            else
            {
                shaderToyMouse =
                {
                    m_mousePosition.x,
                    m_mousePosition.y,
                    -clickX,
                    -clickY
                };
            }
        }

        m_material.setVec4("iMouse", shaderToyMouse);

        // --------------------------------------------------
        // Date
        // --------------------------------------------------

        m_material.setVec4("iDate", getShaderToyDate());
    }

    void VulkanShaderToy::render()
    {
        renderer().drawFullscreen(m_material);
        ++m_frame;
    }

}