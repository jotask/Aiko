#pragma once

#include <aiko_types.h>

#include <types/texture_types.h>
#include "interfaces/itexture_impl.h"

namespace aiko::renderer::vulkan
{

    class VulkanTextureImpl : public interfaces::ITextureImpl
    {
    public:

        VulkanTextureImpl();
        virtual ~VulkanTextureImpl() override = default;

        virtual uint id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;
        virtual TextureInfo getInfo() override;

        // load
        virtual void create(const TextureDesc&) override;
        virtual void unload() override;

        // Modify
        virtual void update(const TextureAsset& asset) override;
        virtual void setPixels(const vector<Color>& pixels) override;

    private:


    };
}
