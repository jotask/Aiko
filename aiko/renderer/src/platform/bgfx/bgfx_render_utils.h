#ifdef AIKO_BGFX

#pragma once

#include <stdint.h>
#include <vector>

#include <bgfx/bgfx.h>

#include "models/mesh.h"

namespace aiko
{
	namespace bgfx::shared
	{

        struct VertexInformation
        {
            float x, y, z;   // position
            float u, v;      // texcoord
            float n_x, n_y, n_z;   // position
            uint32_t abgr;   // color
        };

        static uint64_t default_state = BGFX_STATE_DEFAULT;

        extern ::bgfx::VertexLayout s_global_layout;

        void init();

        std::vector<VertexInformation>  convertToBgfxVertex(const Mesh& mesh);
        std::vector<uint16_t>           convertToBgfxIndices(const Mesh& mesh);

	}
}

#endif