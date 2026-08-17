#pragma once

#include <aiko_types.h>

#include <platform/vulkan/vulkan_types.h>
#include "interfaces/imesh_impl.h"
#include "models/texture.h"

namespace aiko::renderer::vulkan
{

    class VulkanMeshImpl : public interfaces::IMeshImpl
    {
    public:

        explicit VulkanMeshImpl(const Mesh* mesh);
        virtual ~VulkanMeshImpl() override = default;

        virtual bool isValid() const override;
        virtual void unload() override;
        virtual void refresh(const MeshAsset&) override;
        virtual uint id() override;

        Texture* getTexture() { return &m_texture; }

    private:

        Texture m_texture;

    };
}
