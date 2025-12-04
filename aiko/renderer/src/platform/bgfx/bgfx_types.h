#pragma once

#include <bgfx/bgfx.h>

namespace aiko
{

    struct VertexInformation
    {
        float x, y, z;   // position
        float u, v;      // texcoord
        float n_x, n_y, n_z;   // position
        uint32_t abgr;   // color
    };

    inline uint64_t s_default_state = BGFX_STATE_DEFAULT;

    inline ::bgfx::VertexLayout s_global_layout = []{
        ::bgfx::VertexLayout layout;
        layout
            .begin()
            .add(::bgfx::Attrib::Position, 3, ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::TexCoord0, 2, ::bgfx::AttribType::Float)
            .add(::bgfx::Attrib::Normal, 3, ::bgfx::AttribType::Float, true, true)
            .add(::bgfx::Attrib::Color0, 4, ::bgfx::AttribType::Uint8, true)
            .end();
        return layout;
    }();

}