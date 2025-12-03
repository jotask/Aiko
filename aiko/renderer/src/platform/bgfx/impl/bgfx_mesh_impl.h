#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include <models/mesh.h>
#include "interfaces/imesh_impl.h"

namespace aiko::bgfx
{

    class BgfxMeshImpl : public interfaces::IMeshImpl
    {
    public:
        explicit BgfxMeshImpl(const Mesh* mesh);
        virtual ~BgfxMeshImpl() override = default;

        virtual bool isValid() const override;
        virtual void unload() override;
        virtual void refresh() override;

    private:

        struct VertexInformation
        {
            float x, y, z;   // position
            float u, v;      // texcoord
            float n_x, n_y, n_z;   // position
            uint32_t abgr;   // color
        };

        ::bgfx::VertexLayout m_global_layout;

        ::bgfx::VertexBufferHandle m_vertexBuffer;
        ::bgfx::IndexBufferHandle m_indexBuffer;

        std::vector<VertexInformation> convertToVBH();
        std::vector<uint16_t> convertToIBH();

    };
}
