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
        virtual ~VulkanMeshImpl() override;

        virtual bool isValid() const override;
        virtual void unload() override;
        virtual void refresh(const MeshAsset&) override;
        virtual RenderResourceId id() override;

        Texture* getTexture() { return &m_texture; }

        VkBuffer vertexBuffer() const { return m_vertexBuffer; }
        VkBuffer indexBuffer() const { return m_indexBuffer; }
        u32 indexCount() const { return m_indexCount; }

        VkBuffer skinBuffer() const { return m_skinBuffer; }
        bool hasSkinning() const { return m_hasSkinning; }

    private:

        VkBuffer m_vertexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_vertexMemory = VK_NULL_HANDLE;
        VkBuffer m_indexBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_indexMemory = VK_NULL_HANDLE;
        VkBuffer m_skinBuffer = VK_NULL_HANDLE;
        VkDeviceMemory m_skinMemory = VK_NULL_HANDLE;
        u32 m_indexCount = 0;

        Texture m_texture;

        bool m_hasSkinning = false;

    };
}
