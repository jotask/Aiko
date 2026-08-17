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
        virtual ~VulkanComputeShaderImpl() override = default;

        virtual bool isValid() const override;

        // load
        virtual void load(string file) override;
        virtual void unload() override;

        virtual uint id() override;

    private:

        string m_file;

    };
}
