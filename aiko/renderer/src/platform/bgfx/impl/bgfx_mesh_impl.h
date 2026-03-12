#pragma once

#include <aiko_types.h>
#include <bgfx/bgfx.h>

#include <platform/bgfx/bgfx_types.h>
#include "interfaces/imesh_impl.h"
#include "models/texture.h"

namespace aiko::renderer::bgfx
{

    class BgfxMeshImpl : public interfaces::IMeshImpl
    {
    public:
        explicit BgfxMeshImpl(const Mesh* mesh);
        virtual ~BgfxMeshImpl() override = default;

        virtual bool isValid() const override;
        virtual void unload() override;
        virtual void refresh(const MeshAsset&) override;
        virtual uint id() override;

        std::vector<VertexInformation> convertToVBH(const MeshAsset&);
        std::vector<uint16_t> convertToIBH(const MeshAsset&);

        ::bgfx::DynamicVertexBufferHandle getVertexBuffferHandler() const { return m_vertexBuffer; }
        ::bgfx::DynamicIndexBufferHandle getIndexBuffferHandler() const { return m_indexBuffer; }

        Texture* getTexture() { return &m_texture; }

    private:

        ::bgfx::DynamicVertexBufferHandle m_vertexBuffer;
        ::bgfx::DynamicIndexBufferHandle m_indexBuffer;

        Texture m_texture;

    };
}
