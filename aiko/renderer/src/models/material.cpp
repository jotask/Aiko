#include "material.h"

#include <core/utils.h>

#include <utility>

namespace aiko
{

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
        utils::hashCombine(std::hash<AssetId>{}(m_diffuseTextureId), seed);
        utils::hashCombine(std::hash<const Texture*>{}(m_runtimeDiffuseTexture), seed);
        utils::hashCombine(std::hash<bool>{}(m_useVertexColor), seed);
        utils::hashCombine(std::hash<bool>{}(m_lit), seed);
        return static_cast<MaterialId>(seed);
    }

    void Material::setUniform(const string& name, UniformValue value)
    {
        AIKO_ASSERT(name.empty() == false, "Uniform name cannot be empty");
        m_uniforms.insert_or_assign(name, std::move(value));
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

}
