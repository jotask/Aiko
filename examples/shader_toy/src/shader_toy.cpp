#include "shader_toy.h"

#include <chrono>

#include "shared/math.h"
#include "models/game_object.h"
#include "components/camera_component.h"
#include "components/mesh_component.h"
#include "components/light_component.h"
#include "core/utils.h"
#include "core/inputs.h"

namespace shadertoy
{

    void ShaderToy::init()
    {
        Application::init();

        iTime = 0.0f;

        currentShader = 0; // GetRandomValue(0, s_shaders.size());
        refreshShader(true);

    }

    void ShaderToy::update()
    {

        if (isKeyPressed(aiko::KEY_SPACE) || isKeyPressed(aiko::KEY_RIGHT))
        {
            nextShader();
        }
        else if (isKeyPressed(aiko::KEY_LEFT))
        {
            prevShader();
        }
        else if (isKeyPressed(aiko::KEY_R))
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

        m_shader.setShaderValue(iResolutionLoc, &getDisplaySize(), aiko::SHADER_UNIFORM_VEC3);   // viewport resolution (in pixels)
        m_shader.setShaderValue(iTimeLoc, iTime);                                   // shader playback time (in seconds)
        m_shader.setShaderValue(iTimeDeltaLoc, iTimeDelta);                         // render time (in seconds)
        m_shader.setShaderValue(iFrameRateLoc, iFrameRate);                         // shader frame rate
        m_shader.setShaderValue(iFrameLoc, iFrame);                                   // shader playback frame

        m_shader.setShaderValueV(iChannelTimeLoc, iChannelTime, aiko::shader::Shader::SUDT::SHADER_UNIFORM_FLOAT, 4);                     // channel playback time (in seconds)
        m_shader.setShaderValue(iChannelResolutionLoc, &iChannelResolution, aiko::SHADER_UNIFORM_VEC3);          // channel resolution (in pixels)
        m_shader.setShaderValue(iMouseLoc, &iMouse, aiko::SHADER_UNIFORM_VEC4);                                  // mouse pixel coords. xy: current (if MLB down), zw: click
        m_shader.setShaderValue(iDateLoc, &iDate, aiko::SHADER_UNIFORM_VEC4);                                    // (year, month, day, time in seconds)

    }

    void ShaderToy::render()
    {
        auto* texture = getTargetTexture();
        m_shader.draw(texture);
    }

    std::vector<std::string> ShaderToy::s_shaders =
    {
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
        "aiko_shadertoy",
        "aiko_shadertoy_fractal",
        "aiko_shadertoy_fractal_infinite_1",
        "aiko_shadertoy_fractal_infinite_2",
        "aiko_shadertoy_fractal_infinite_spheres",
        "aiko_shadertoy_happy",
    };

    void ShaderToy::nextShader()
    {
        currentShader++;
        currentShader %= s_shaders.size();
        refreshShader();
    }

    void ShaderToy::randomShader()
    {
        currentShader = aiko::utils::getRandomValue(0, s_shaders.size());
        refreshShader();
    }

    void ShaderToy::prevShader()
    {
        currentShader--;
        currentShader %= s_shaders.size();
        refreshShader();
    }

    void ShaderToy::refreshShader(bool first)
    {
        if (first == false)
        {
            m_shader.unload();
        }

        const std::string path = GLOBAL_PATH + s_shaders[currentShader].c_str() + EXTENSION;
        m_shader.load(nullptr, path.c_str());

        iResolutionLoc = m_shader.getShaderLocation("iResolution");
        iTimeLoc = m_shader.getShaderLocation("iTime");
        iTimeDeltaLoc = m_shader.getShaderLocation("iTimeDelta");
        iFrameRateLoc = m_shader.getShaderLocation("iFrameRate");
        iFrameLoc = m_shader.getShaderLocation("iFrame");
        iChannelTimeLoc = m_shader.getShaderLocation("iChannelTime");
        iChannelResolutionLoc = m_shader.getShaderLocation("iChannelResolution");
        iMouseLoc = m_shader.getShaderLocation("iMouse");
        iDateLoc = m_shader.getShaderLocation("iDate");

        // assert(positionLoc != -1 && nParticlesLoc != -1 && resolutionLoc != -1 && radiusLoc != -1 );
        // assert(iResolutionLoc != -1 && iTimeLoc != -1 && iTimeDeltaLoc != -1 && iFrameRateLoc != -1 && iFrameLoc != -1 && iChannelTimeLoc != -1 && iChannelResolutionLoc != -1 && iMouseLoc != -1 && iDateLoc != -1);

    }

}
