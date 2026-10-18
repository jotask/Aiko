 #pragma once

#include "assets/asset_id.h"
#include "types/color.h"
#include "types/render_types.h"
#include "types/uniform_value.h"
#include "types/render_state.h"

#include <metadata/texture_meta.h>

 namespace aiko
{
     class ComputeBuffer;
     class Texture;

     struct TextureBinding
     {
         AssetId textureId = InvalidAssetId;
         const Texture* runtimeTexture = nullptr;
         SamplerState sampler{};
         TextureColorSpace colorSpace = TextureColorSpace::Linear;

         bool operator==(const TextureBinding& other) const = default;
     };

    class Material
    {
    public:

        friend class RenderModule;
        friend class RenderSystem;

        Material(const Material&) = default;
        Material& operator=(const Material&) = default;

        Material(Material&&) noexcept = default;
        Material& operator=(Material&&) noexcept = default;

        Material();
        ~Material() = default;

        MaterialId id() const;

        void setUniform(const string& name, UniformValue value);

        void setTexture(const string& name, AssetId textureId);
        void setTexture(const string& name, AssetId textureId, const SamplerState& sampler);

        void setTexture(const string& name, const Texture* texture);
        void setTexture(const string& name, const Texture* texture, const SamplerState& sampler);

        void clearTexture(const string& name);

        const TextureBinding* textureBinding(const string& name) const;

        const std::unordered_map<string, TextureBinding>& textureBindings() const
        {
            return m_textureBindings;
        }

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

        void setFloatArray(const string& name, vector<float> values);
        void setVec3Array(const string& name, vector<vec3> values);

        void setTextureSampler(const string& name, const SamplerState& sampler);

        const UniformMap& uniforms() const
        {
            return m_uniforms;
        }

        bool hasSameState(const Material& other) const;

        void setAlbedoMap(AssetId textureId);
        void setMetallicRoughnessMap(AssetId textureId);

        void setSpecular(float strength, float shininess);
        float specularStrength() const { return m_specularStrength; }
        float shininess() const { return m_shininess; }

        void setMetallic(float metallic);
        void setRoughness(float roughness);

        float metallic() const { return m_metallic; }
        float roughness() const { return m_roughness; }

    public:

        AssetId m_shaderId = InvalidAssetId;

        const ComputeBuffer* m_gpuInstanceBuffer = nullptr;

        RenderState m_renderState;
        bool m_useVertexColor;
        bool m_lit;
        float m_pointSize = 4.0f;
        float m_lineWidth = 1.0f;

        Color m_baseColor;

    private:

        UniformMap m_uniforms;
        std::unordered_map<string, TextureBinding> m_textureBindings;

        float m_specularStrength = 0.0f;
        float m_shininess = 32.0f;

        float m_metallic = 0.0f;
        float m_roughness = 0.5f;

    };

}
