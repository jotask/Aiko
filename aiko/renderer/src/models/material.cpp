#include "material.h"

#include <stdexcept>

namespace aiko
{

    Material::Material()
		: m_baseColor(RED)
        , m_userVertexColor(true)
        , m_lit(false)

    {

    }

    u64 Material::id() const
    {

        const u64 shaderId = static_cast<u64>(m_shader.id());
        const u64 textureId = m_diffuse.isValid() ? static_cast<u64>(m_diffuse.id()) : 0ull;

        u64 flags = 0;
        if (m_userVertexColor == true) flags    |= 1ull << 0;
        if (m_lit == true) flags                |= 1ull << 1;

        u64 id = 0;
        id |= (shaderId & 0xffffull) << 48;
        id |= (textureId & 0xffffffffull) << 16;
        id |= (flags & 0xffffull);

        return id;
    }

}
