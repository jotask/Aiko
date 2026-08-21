#pragma once

#include <unordered_map>

#include <aiko_types.h>

#include "interfaces/icomputeshader_impl.h"

namespace aiko::renderer::vulkan
{

    class VulkanComputeShaderImpl : public interfaces::IComputeShaderImpl
    {
    public:

        VulkanComputeShaderImpl();
        virtual ~VulkanComputeShaderImpl() override;

        virtual bool isValid() const override;

        // load
        virtual void load(string file) override;
        virtual void unload() override;

        virtual RenderResourceId id() override;

        VkShaderModule module() const { return m_module; }

    private:

        VkShaderModule m_module = VK_NULL_HANDLE;
        string m_file;

    };
}
