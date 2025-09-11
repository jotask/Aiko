#ifdef AIKO_BGFX

#include <stdint.h>
#include <vector>

#include <bgfx/bgfx.h>

#include "models/mesh.h"

namespace aiko
{
	namespace bgfx
	{

        struct VertexInformation
        {
            float x, y, z;   // position
            float u, v;      // texcoord
            uint32_t abgr;   // color
        };

        static ::bgfx::VertexLayout s_global_layout;

        void init();

        std::vector<VertexInformation> convertTo(const Mesh& mesh);

	}
}

#endif