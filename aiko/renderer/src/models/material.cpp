#include "material.h"

#include <core/utils.h>

namespace aiko
{

    Material::Material()
		: m_baseColor(WHITE)
        , m_useVertexColor(true)
        , m_lit(false)

    {

    }

    u64 Material::id() const
    {
        std::size_t seed = 0;
        utils::hashCombine(std::hash<AssetId>{}(m_shaderId), seed);
        utils::hashCombine(std::hash<AssetId>{}(m_diffuseTextureId), seed);
        utils::hashCombine(std::hash<bool>{}(m_useVertexColor), seed);
        utils::hashCombine(std::hash<bool>{}(m_lit), seed);
        return static_cast<u64>(seed);
    }

}
