#pragma once

#include "core/libs.h"
#include "aiko_types.h"
#include "camera_types.h"

namespace aiko
{

    class ShaderPtr
    {
    public:
        friend class RenderSystem;

        using SUDT = ShaderUniformDataType;

        ShaderPtr();
        ~ShaderPtr() = default;

        void load(const char* vs, const char* fs);
        void unload();

        int getShaderLocation(const char* locName);

        void setShaderValueV(int locIndex, const void* value, SUDT uniformType, int count);

        void setShaderValue(int locIndex, const void* value, SUDT uniformType);

        // Type Helpers

        void setShaderValue(int locIndex, const int& value);
        void setShaderValue(int locIndex, const float& value);

        void draw(RenderTexture2D* texture);

    private:

        Shader m_shader;
    
    };

}
