#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include <platform/bgfx/bgfx_types.h>
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

        ::bgfx::VertexBufferHandle getVertexBuffferHandler() const { return m_vertexBuffer; }
        ::bgfx::IndexBufferHandle getIndexBuffferHandler() const { return m_indexBuffer; }

    private:

        ::bgfx::VertexBufferHandle m_vertexBuffer;
        ::bgfx::IndexBufferHandle m_indexBuffer;

        std::vector<VertexInformation> convertToVBH();
        std::vector<uint16_t> convertToIBH();

    };
}
