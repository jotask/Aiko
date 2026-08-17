#include "vulkan_shader_impl.h"

#include <core/file.h>

#include "platform/vulkan/vulkan_platform_helper.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    VulkanShaderImpl::VulkanShaderImpl()
    {
    }

    void VulkanShaderImpl::load(const char* vs, const char* fs)
    {
        vertex_file = string(vs);
        fragment_file = string(fs);
    }

    void VulkanShaderImpl::unload()
    {

    }


    uint VulkanShaderImpl::id() const
    {
        return {};
    }

    void VulkanShaderImpl::use()
    {

    }

    void VulkanShaderImpl::unuse()
    {

    }

    bool VulkanShaderImpl::isValid() const
    {
        return false;
    }

    void VulkanShaderImpl::setBool(const string& name, bool value)
    {

    }

    void VulkanShaderImpl::setInt(const string& name, int value)
    {

    }

    void VulkanShaderImpl::setFloat(const string& name, float value)
    {

    }

    void VulkanShaderImpl::setVec2(const string& name, const vec2& value)
    {

    }

    void VulkanShaderImpl::setVec3(const string& name, const vec3& value)
    {

    }

    void VulkanShaderImpl::setVec4(const string& name, const vec4& value)
    {

    }

    void VulkanShaderImpl::setMat4(const string& name, const mat4& mat)
    {

    }

    void VulkanShaderImpl::setVec4Array(const string& name, const vec4* values, uint32_t count)
    {

    }

    bool VulkanShaderImpl::hasUniform(const string& name) const
    {
        return false;
    }
}
