#pragma once

#include <aiko_types.h>
#include <math/math.h>

#include <types/texture_types.h>

#include "interfaces/iframebuffer_impl.h"
#include "models/texture.h"

namespace aiko::renderer::vulkan
{

    class VulkanFrameBufferImpl : public interfaces::IFrameBufferImpl
    {
    public:
        VulkanFrameBufferImpl();

        virtual ~VulkanFrameBufferImpl() override = default;

        virtual uint id() const override;

        virtual void use() override;

        virtual void unuse() override;

        virtual bool isValid() const override;

        // load
        virtual void create(Texture& color, Texture& depth) override;

        virtual void unload() override;

    private:


    };
}
