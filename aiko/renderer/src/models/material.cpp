#include "material.h"

#include <stdexcept>

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

        const u64 shaderId = (m_shader != nullptr && m_shader->isValid()) ? static_cast<u64>(m_shader->id()) : 0ull;
        const u64 textureId = m_diffuseTexture != nullptr && m_diffuseTexture->isValid() ? static_cast<u64>(m_diffuseTexture->id()) : 0ull;

        u64 flags = 0;
        if (m_useVertexColor == true) flags     |= 1ull << 0;
        if (m_lit == true) flags                |= 1ull << 1;

        u64 id = 0;
        id |= (shaderId & 0xffffull) << 48;
        id |= (textureId & 0xffffffffull) << 16;
        id |= (flags & 0xffffull);

        return id;
    }

}
