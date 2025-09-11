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
            uint32_t abgr;   // color
        };

        static uint64_t default_state = 0
            | BGFX_STATE_WRITE_R
            | BGFX_STATE_WRITE_G
            | BGFX_STATE_WRITE_B
            | BGFX_STATE_WRITE_A
            | BGFX_STATE_WRITE_Z
            | BGFX_STATE_DEPTH_TEST_LESS
            // | BGFX_STATE_CULL_CW
            | BGFX_STATE_MSAA
        ;

        extern ::bgfx::VertexLayout s_global_layout;

        void init();

        std::vector<VertexInformation> convertToBgfxVertex(const Mesh& mesh);

	}
}

#endif