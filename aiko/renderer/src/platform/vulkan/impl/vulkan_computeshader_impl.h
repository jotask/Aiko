#pragma once

#include <unordered_map>

#include <aiko_types.h>

#include "interfaces/icomputeshader_impl.h"
#include "platform/vulkan/vulkan_shader_reflection.h"

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

        virtual RenderResourceId id() const override;

        VkShaderModule module() const { return m_module; }
        const VulkanShaderReflection& reflection() const { return m_reflection; }

    private:

        string m_file;
        VkShaderModule m_module = VK_NULL_HANDLE;
        VulkanShaderReflection m_reflection;

    };
}
