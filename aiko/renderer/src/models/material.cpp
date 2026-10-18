#include "material.h"

#include <core/utils.h>

#include <utility>
#include <type_traits>
#include <cmath>

namespace aiko
{

    namespace
    {
        bool sameUniformValue(const UniformValue& a, const UniformValue& b)
        {
            if (a.index() != b.index())
            {
                return false;
            }

            return std::visit(
                [&b](const auto& value) -> bool
                {
                    using T = std::decay_t<decltype(value)>;
                    const T& other = std::get<T>(b);

                    if constexpr (std::is_arithmetic_v<T>)
                    {
                        return value == other;
                    }
                    else if constexpr (std::is_same_v<T, mat4>)
                    {
                        for (int i = 0; i < 16; ++i)
                        {
                            if (value.data()[i] != other.data()[i])
                            {
                                return false;
                            }
                        }

                        return true;
                    }
                    else if constexpr (std::is_same_v<T, FloatArray>)
                    {
                        return value.values == other.values;
                    }
                    else if constexpr (std::is_same_v<T, Vec3Array>)
                    {
                        if (value.values.size() != other.values.size())
                        {
                            return false;
                        }

                        for (size_t i = 0; i < value.values.size(); ++i)
                        {
                            const vec3& left = value.values[i];
                            const vec3& right = other.values[i];

                            if (left.x != right.x || left.y != right.y || left.z != right.z)
                            {
                                return false;
                            }
                        }

                        return true;
                    }
                    else if constexpr (std::is_same_v<T, bvec2> || std::is_same_v<T, ivec2> || std::is_same_v<T, uvec2> || std::is_same_v<T, vec2>)
                    {
                        return value.x == other.x && value.y == other.y;
                    }
                    else if constexpr (std::is_same_v<T, bvec3> || std::is_same_v<T, ivec3> || std::is_same_v<T, uvec3> || std::is_same_v<T, vec3>)
                    {
                        return value.x == other.x && value.y == other.y && value.z == other.z;
                    }
                    else
                    {
                        return value.x == other.x && value.y == other.y && value.z == other.z && value.w == other.w;
                    }
                },
                a);
        }
    }

    Material::Material()
		: m_baseColor(WHITE)
        , m_useVertexColor(true)
        , m_lit(false)
    {

    }

    MaterialId Material::id() const
    {
        std::size_t seed = 0;
        utils::hashCombine(std::hash<AssetId>{}(m_shaderId), seed);
        utils::hashCombine(std::hash<bool>{}(m_useVertexColor), seed);
        utils::hashCombine(std::hash<bool>{}(m_lit), seed);
        utils::hashCombine(std::hash<float>{}(m_specularStrength), seed);
        utils::hashCombine(std::hash<float>{}(m_shininess), seed);
        utils::hashCombine(std::hash<u32>{}(m_baseColor.rgba()), seed);
        utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(m_renderState.cullMode)), seed);
        utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(m_renderState.fillMode)), seed);
        utils::hashCombine(std::hash<bool>{}(m_renderState.depthTest), seed);
        utils::hashCombine(std::hash<bool>{}(m_renderState.depthWrite), seed);
        utils::hashCombine(std::hash<uint32_t>{}(static_cast<uint32_t>(m_renderState.depthCompare)), seed);
        utils::hashCombine(std::hash<bool>{}(m_renderState.blend), seed);
        return static_cast<MaterialId>(seed);
    }

    void Material::setUniform(const string& name, UniformValue value)
    {
        AIKO_ASSERT(name.empty() == false, "Uniform name cannot be empty");
        m_uniforms.insert_or_assign(name, std::move(value));
    }

    void Material::setTexture(const string& name, AssetId textureId)
    {
        AIKO_ASSERT(name.empty() == false, "Texture binding name cannot be empty");
        TextureBinding& binding = m_textureBindings[name];
        binding.textureId = textureId;
        binding.runtimeTexture = nullptr;
    }

    void Material::setBool(const string& name, bool value)
    {
        setUniform(name, value);
    }

    void Material::setInt(const string& name, int value)
    {
        setUniform(name, value);
    }

    void Material::setUInt(const string& name, u32 value)
    {
        setUniform(name, value);
    }

    void Material::setFloat(const string& name, float value)
    {
        setUniform(name, value);
    }

    void Material::setVec2(const string& name, const vec2& value)
    {
        setUniform(name, value);
    }

    void Material::setVec2(const string& name, float x, float y)
    {
        setUniform(name, vec2{x, y});
    }

    void Material::setVec3(const string& name, const vec3& value)
    {
        setUniform(name, value);
    }

    void Material::setVec3(const string& name, float x, float y, float z)
    {
        setUniform(name, vec3{x, y, z});
    }

    void Material::setVec4(const string& name, const vec4& value)
    {
        setUniform(name, value);
    }

    void Material::setVec4(const string& name, float x, float y, float z, float w
    )
    {
        setUniform(name, vec4{x, y, z, w});
    }

    void Material::setMat4(const string& name, const mat4& value)
    {
        setUniform(name, value);
    }

    void Material::setFloatArray(const string& name, vector<float> values)
    {
        setUniform( name, FloatArray { .values = std::move(values) });
    }

    void Material::setVec3Array(const string& name, vector<vec3> values)
    {
        setUniform(name, Vec3Array { .values = std::move(values) });
    }

    void Material::setTextureSampler(const string& name, const SamplerState& sampler)
    {
        AIKO_ASSERT(name.empty() == false, "Texture binding name cannot be empty");
        auto [it, inserted] = m_textureBindings.try_emplace(name);
        AIKO_UNUSED(inserted);
        it->second.sampler = sampler;
    }

    bool Material::hasSameState(const Material& other) const
    {
        if (m_shaderId != other.m_shaderId ||
            m_gpuInstanceBuffer != other.m_gpuInstanceBuffer ||
            m_useVertexColor != other.m_useVertexColor ||
            m_lit != other.m_lit ||
            m_specularStrength != other.m_specularStrength ||
            m_shininess != other.m_shininess ||
            m_pointSize != other.m_pointSize ||
            m_lineWidth != other.m_lineWidth ||
            m_baseColor.r != other.m_baseColor.r ||
            m_baseColor.g != other.m_baseColor.g ||
            m_baseColor.b != other.m_baseColor.b ||
            m_baseColor.a != other.m_baseColor.a)
        {
            return false;
        }

        const RenderState& a = m_renderState;
        const RenderState& b = other.m_renderState;

        if (a.cullMode != b.cullMode ||
            a.fillMode != b.fillMode ||
            a.depthTest != b.depthTest ||
            a.depthWrite != b.depthWrite ||
            a.depthCompare != b.depthCompare ||
            a.blend != b.blend)
        {
            return false;
        }

        if (m_textureBindings != other.m_textureBindings || m_uniforms.size() != other.m_uniforms.size())
        {
            return false;
        }

        for (const auto& [name, value] : m_uniforms)
        {
            const auto it = other.m_uniforms.find(name);

            if (it == other.m_uniforms.end() || !sameUniformValue(value, it->second))
            {
                return false;
            }
        }

        return true;
    }

    void Material::setSpecular(float strength, float shininess)
    {
        AIKO_ASSERT(std::isfinite(strength) && strength >= 0.0f, "Specular strength must be finite and non-negative");
        AIKO_ASSERT(std::isfinite(shininess) && shininess >= 1.0f, "Specular shininess must be finite and at least 1");
        m_specularStrength = strength;
        m_shininess = shininess;
    }

    void Material::setTexture(const string& name, AssetId textureId, const SamplerState& sampler)
    {
        AIKO_ASSERT(name.empty() == false, "Texture binding name cannot be empty");
        m_textureBindings.insert_or_assign(
            name,
            TextureBinding
            {
                .textureId = textureId,
                .runtimeTexture = nullptr,
                .sampler = sampler,
            }
        );
    }

    void Material::setTexture(const string& name, const Texture* texture)
    {
        AIKO_ASSERT(name.empty() == false, "Texture binding name cannot be empty");
        TextureBinding& binding = m_textureBindings[name];
        binding.textureId = InvalidAssetId;
        binding.runtimeTexture = texture;
    }

    void Material::setTexture(const string& name, const Texture* texture, const SamplerState& sampler)
    {
        AIKO_ASSERT(name.empty() == false, "Texture binding name cannot be empty");

        m_textureBindings.insert_or_assign(
            name,
            TextureBinding
            {
                .textureId = InvalidAssetId,
                .runtimeTexture = texture,
                .sampler = sampler,
            }
        );
    }

    void Material::clearTexture(const string& name)
    {
        m_textureBindings.erase(name);
    }

    const TextureBinding* Material::textureBinding(const string& name) const
    {
        const auto it = m_textureBindings.find(name);
        if (it == m_textureBindings.end())
        {
            return nullptr;
        }
        return &it->second;
    }

}
