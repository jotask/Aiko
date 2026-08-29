#pragma once

#include <aiko_types.h>
#include <math/math_vector.h>

#include "interfaces/ishader_impl.h"
#include "platform/vulkan/vulkan_shader_reflection.h"

namespace aiko::renderer::vulkan
{

    class VulkanShaderImpl : public interfaces::IShaderImpl
    {
    public:

        VulkanShaderImpl();
        virtual ~VulkanShaderImpl() override;

        virtual RenderResourceId id() const override;
        virtual void use() override;
        virtual void unuse() override;
        virtual bool isValid() const override;

        virtual void load(const char*, const char*) override;
        virtual void unload() override;

        virtual void setBool(const string& name, bool value) override;
        virtual void setInt(const string& name, int value) override;
        virtual void setFloat(const string& name, float value) override;
        virtual void setVec2(const string& name, const vec2& value) override;
        virtual void setVec3(const string& name, const vec3& value) override;
        virtual void setVec4(const string& name, const vec4& value) override;
        virtual void setMat4(const string& name, const mat4& mat) override;
        virtual void setVec4Array(const string& name, const vec4* values, uint32_t count) override;

        bool hasUniform(const string& name) const;

        VkShaderModule vertexModule() const { return m_vertexModule; }
        VkShaderModule fragmentModule() const { return m_fragmentModule; }
        const VulkanShaderReflection& reflection() const { return m_reflection; }

    private:

        string vertex_file;
        string fragment_file;

        VkShaderModule m_vertexModule = VK_NULL_HANDLE;
        VkShaderModule m_fragmentModule = VK_NULL_HANDLE;

        VulkanShaderReflection m_reflection;

        VkShaderModule createShaderModule(VkDevice device, const vector<uint8_t>& code);

    };
}
