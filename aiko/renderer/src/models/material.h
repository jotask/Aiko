 #pragma once

#include "models/shader.h"
#include "models/texture.h"
#include "types/color.h"
#include "types/render_types.h"
#include "types/uniform_value.h"
#include <metadata/texture_meta.h>

 namespace aiko
{
    class ComputeBuffer;

    class Material
    {
    public:

        friend class RenderModule;
        friend class RenderSystem;

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        Material(Material&&) noexcept = default;
        Material& operator=(Material&&) noexcept = default;

        Material();
        ~Material() = default;

        MaterialId id() const;

        void setUniform(const string& name, UniformValue value);

        void setBool(const string& name, bool value);
        void setInt(const string& name, int value);
        void setUInt(const string& name, u32 value);
        void setFloat(const string& name, float value);

        void setVec2(const string& name, const vec2& value);
        void setVec2(const string& name, float x, float y);

        void setVec3(const string& name, const vec3& value);
        void setVec3(const string& name, float x, float y, float z);

        void setVec4(const string& name, const vec4& value);
        void setVec4(const string& name, float x, float y, float z, float w);

        void setMat4(const string& name, const mat4& value);

        const UniformMap& uniforms() const
        {
            return m_uniforms;
        }

        void setSamplerState(const SamplerState& state) { m_samplerState = state; }
        const SamplerState& samplerState() const { return m_samplerState; }

    public:

        AssetId m_shaderId = InvalidAssetId;

        const ComputeBuffer* m_gpuInstanceBuffer = nullptr;

        RenderState m_renderState;
        bool m_useVertexColor;
        bool m_lit;

		Color m_baseColor;
        AssetId m_diffuseTextureId = InvalidAssetId;
        const Texture* m_runtimeDiffuseTexture = nullptr;

        SamplerState m_samplerState{};

    private:

        UniformMap m_uniforms;

    };

}
