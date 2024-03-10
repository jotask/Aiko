#pragma once

#include <vector>

#include "aiko_types.h"
#include "types/camera_types.h"
#include "types/render_types.h"
#include "types/textures.h"
#include "types/asset_type.h"
#include "shared/math.h"
#include "types/render_types.h"

namespace aiko
{
    class RenderSystem;
    namespace asset
    {
        class Shader : public Asset
        {
        public:
            friend class RenderSystem;

            Shader();
            ~Shader() = default;

            void load(const char* vs, const char* fs);
            virtual void unload() override;

            int getShaderLocation(const char* locName);

            // Type Helpers

            void setShaderValue(int locIndex, const int& value);
            void setShaderValue(int locIndex, const float& value);
            void setShaderValue(int locIndex, const ivec2& value);
            void setShaderValue(int locIndex, const vec2& value);
            void setShaderValue(int locIndex, const vec3& value);
            void setShaderValue(int locIndex, const vec4& value);
            void setShaderValue(int locIndex, const std::vector<vec2>& value);

        protected:
            virtual void connect();

        private:
            RenderSystem* m_renderSystem;
            aiko::ShaderData m_shaderData;

        };
    }
}
