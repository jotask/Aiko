#pragma once

#include <vector>

#include "aiko_types.h"
#include "camera_types.h"
#include "core/render_types.h"
#include "core/textures.h"

namespace aiko::shader
{

    class Shader
    {
    public:
        friend class RenderSystem;

        using SUDT = ShaderUniformDataType;

        Shader();
        ~Shader() = default;

        void load(const char* vs, const char* fs);
        void unload();

        int getShaderLocation(const char* locName);

        void setShaderValueV(int locIndex, const void* value, SUDT uniformType, int count);

        void setShaderValue(int locIndex, const void* value, SUDT uniformType);

        // Type Helpers

        void setShaderValue(int locIndex, const int& value);
        void setShaderValue(int locIndex, const float& value);

        void draw(texture::RenderTexture2D* texture);

    //private:

        // Shader m_shader;
        unsigned int            m_id;
        std::vector<int>        m_locs;
    
    };

}
