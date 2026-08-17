#include "vulkan_texture_impl.h"

#include <algorithm>
#include <random>

#include <logger/logger.h>
#include <core/file.h>

#include "constants.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    VulkanTextureImpl::VulkanTextureImpl()
    {
    }

    uint VulkanTextureImpl::id() const
    {
        return {};
    }

    void VulkanTextureImpl::use()
    {

    }

    void VulkanTextureImpl::unuse()
    {

    }

    bool VulkanTextureImpl::isValid() const
    {

    }

    TextureInfo VulkanTextureImpl::getInfo()
    {
        return {};
    }

    void VulkanTextureImpl::create(const TextureDesc& desc)
    {

    }

    void VulkanTextureImpl::unload()
    {

    }

    void VulkanTextureImpl::update(const TextureAsset& asset)
    {
        setPixels(asset.pixels);
    }

    void VulkanTextureImpl::setPixels(const vector<Color>& pixels)
    {

    }


}
