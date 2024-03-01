#pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/render_types.h"
#include "types/textures.h"

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

    //private:

        using Locs = std::vector<int>;

        // Shader m_shader;
        unsigned int            m_id;
        Locs                    m_locs;
    
    };

}
