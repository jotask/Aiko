#pragma once

#include <string>

#include "application/application.h"
#include "models/shader.h"
#include "types/textures.h"
#include "types/asset_type.h"

namespace shadertoy
{

    class RenderSystem;

    class ShaderToy : public aiko::Application
    {
    protected:
        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

        int iResolutionLoc;
        int iTimeLoc;
        int iTimeDeltaLoc;
        int iFrameRateLoc;
        int iFrameLoc;
        int iChannelTimeLoc;
        int iChannelResolutionLoc;
        int iMouseLoc;
        int iDateLoc;

        const std::string GLOBAL_PATH = "C:/Users/j.iznardo/Documents/Aiko/assets/shaders/";
        const std::string EXTENSION = ".fs";
        static std::vector<std::string> s_shaders;

        aiko::vec3      iResolution;           // viewport resolution (in pixels)
        float           iTime;                 // shader playback time (in seconds)
        float           iTimeDelta;            // render time (in seconds)
        float           iFrameRate;            // shader frame rate
        int             iFrame;                // shader playback frame
        float           iChannelTime[4];       // channel playback time (in seconds)
        aiko::vec3      iChannelResolution[4]; // channel resolution (in pixels)
        aiko::vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
        aiko::vec4      iDate;                 // (year, month, day, time in seconds)

        aiko::AikoPtr<aiko::Shader> m_shader;

        int currentShader = 0;
        void nextShader();
        void prevShader();
        void randomShader();
        void refreshShader();

    };

}
