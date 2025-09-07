#include "shader_toy.h"

#include <chrono>
#include <cassert>

#include "shared/math.h"
#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "systems/render_system.h"
#include "core/utils.h"
#include "types/inputs.h"
#include "models/camera.h"
#include "shared/math.h"
#include <aiko_includes.h>

namespace shadertoy
{

    std::vector<aiko::string> ShaderToy::s_shaders =
    {
        "aiko_shadertoy",
        "aiko_shadertoy_uv",
        "aiko_shadertoy_circle",
        "aiko_shadertoy_old",
        "aiko_shadertoy_mandelbulb",
        "aiko_shadertoy_coastal_fractal",
        "aiko_shadertoy_cyber_fuji",
        "aiko_shadertoy_fractal_pyramid",
        "aiko_shadertoy_phanton_start",
        "aiko_shadertoy_sun",
        "aiko_shadertoy_sun_mandala",
        "aiko_shadertoy_fractal_lines",
        "aiko_shadertoy_mandala_art_of_code",
        "aiko_shadertoy_hexaflower",
        "aiko_shadertoy_rosaic",
        "aiko_shadertoy_test_fractal",
        "aiko_shadertoy_nunu_mandala",
        "aiko_shadertoy_fractal_mandala_twtich",
        "aiko_shadertoy_fractal_circles",
        "aiko_shadertoy_mandala_start_pattern",
        "aiko_shadertoy_weird_twitch",
        "aiko_shadertoy_mandala",
        "aiko_shadertoy_a_nice_mandala",
        "aiko_shadertoy_mandala_flowers",
        "aiko_shadertoy_happy_seigaiha_mandala",
        "aiko_shadertoy_fractal",
        "aiko_shadertoy_fractal_infinite_1",
        "aiko_shadertoy_fractal_infinite_2",
        "aiko_shadertoy_fractal_infinite_spheres",
        "aiko_shadertoy_happy",
    };

    void ShaderToy::init()
    {
        Application::init();

        iTime = 0.0f;
        iFrame = 0;
        
        m_shader = { 0 };
        
        auto camera = this->Instantiate("Camera");
        auto cam = camera->addComponent<aiko::CameraComponent>(aiko::camera::CameraController::Static);
        
        assert( m_shader != nullptr, "shader not init");
        
        currentShader = 0; // aiko::utils::getRandomValue(0, s_shaders.size());
        refreshShader();

    }

    void ShaderToy::update()
    {

        if (isKeyJustPressed(aiko::KEY_SPACE) || isKeyJustPressed(aiko::KEY_RIGHT))
        {
            nextShader();
        }
        else if (isKeyJustPressed(aiko::KEY_LEFT))
        {
            prevShader();
        }
        else if (isKeyJustPressed(aiko::KEY_R))
        {
            randomShader();
        }

        // Please fix me
        {

            iTime += getlDeltaTime();

            iTimeDelta = getlDeltaTime();
            iFrameRate = getlDeltaTime();

            iFrame++;

            auto mouse = getMousePosition();
            iMouse = aiko::vec4(mouse.x, mouse.y, isMouseButtonPressed(aiko::MOUSE_BUTTON_RIGHT), isMouseButtonPressed(aiko::MOUSE_BUTTON_RIGHT));

            typedef std::chrono::system_clock Clock;
            auto now = Clock::now();
            std::time_t now_c = Clock::to_time_t(now);
            struct tm* parts = std::localtime(&now_c);
            iDate = aiko::vec4(1900 + parts->tm_year, 1 + parts->tm_mon, parts->tm_mday, time(NULL));
        }

        if (m_shader->isvalid() == false)
        {
            return;
        }

        // TODO :: Extract this from here, we dont' need to update each frame only when viewport/display changes
        aiko::vec3 iResolution = { getViewportSize(), getAspectRatio() }; // vec3 becasue { width, height, pixel aspect ratio }

        m_shader->setVec3("iResolution", iResolution);
        m_shader->setFloat("iTime", iTime);                                   // shader playback time (in seconds)
        m_shader->setFloat("iTimeDelta", iTimeDelta);                         // render time (in seconds)
        m_shader->setFloat("iFrameRate", iFrameRate);                         // shader frame rate
        m_shader->setInt("iFrame", iFrame);                                 // shader playback frame

        // m_shader->setShaderValueV(iChannelTimeLoc, iChannelTime, aiko::ShaderUniformDataType::SHADER_UNIFORM_FLOAT, 4);                     // channel playback time (in seconds)
        // m_shader->setShaderValue(iChannelResolutionLoc, &iChannelResolution, aiko::SHADER_UNIFORM_VEC3);          // channel resolution (in pixels)
        m_shader->setVec4("iMouse", iMouse);                                  // mouse pixel coords. xy: current (if MLB down), zw: click
        m_shader->setVec4("iDate", iDate);                                    // (year, month, day, time in seconds)

    }

    void ShaderToy::render()
    {
        if (m_shader->isvalid() == true)
        {
            auto* renderer = getRenderSystem();
            renderer->renderToFullScreen(m_shader.get());
        }
    }

    void ShaderToy::nextShader()
    {
        currentShader++;
        currentShader %= s_shaders.size();
        refreshShader();
    }

    void ShaderToy::randomShader()
    {
        currentShader = aiko::utils::getRandomValue(0, s_shaders.size() - 1);
        refreshShader();
    }

    void ShaderToy::prevShader()
    {
        currentShader--;
        currentShader %= s_shaders.size();
        refreshShader();
    }

    void ShaderToy::refreshShader()
    {
        m_shader->load("aiko_shadertoy.vs", "aiko_shadertoy.fs");
    }

}
