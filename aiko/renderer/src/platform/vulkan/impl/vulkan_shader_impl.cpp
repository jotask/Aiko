#include "vulkan_shader_impl.h"

#include "platform/vulkan/vulkan_context.h"

#include <core/file.h>

#include "platform/vulkan/vulkan_platform_helper.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{

    VulkanShaderImpl::VulkanShaderImpl()
    {
    }

    VulkanShaderImpl::~VulkanShaderImpl()
    {
        AIKO_ASSERT(m_vertexModule == VK_NULL_HANDLE && m_fragmentModule == VK_NULL_HANDLE, "VulkanShaderImpl destroyed without unload()");
    }

    void VulkanShaderImpl::load(const char* vs, const char* fs)
    {

        unload();

        auto replacePrefix = [](const char* file, const char* ext) -> string
        {
            std::filesystem::path path(file);
            path.replace_extension(ext);
            return path.generic_string();
        };

        vertex_file = replacePrefix(vs, ".vert");
        fragment_file = replacePrefix(fs, ".frag");

        using stdpath = std::filesystem::path;

        const stdpath base = stdpath(global::GLOBAL_PATH) / "build/assets/shaders/vulkan";
        const stdpath vshaderPath = base / (vertex_file + std::string(".spv"));
        const stdpath fshaderPath = base / (fragment_file + std::string(".spv"));

        AIKO_ASSERT(std::filesystem::exists(vshaderPath), "Vertex shader file not found!");
        AIKO_ASSERT(std::filesystem::exists(fshaderPath), "Fragment shader file not found!");

        auto vertShaderCode = files::readFileBytes(vshaderPath.c_str());
        auto fragShaderCode = files::readFileBytes(fshaderPath.c_str());

        const VulkanContext& ctx = VulkanContext::current();

        m_vertexModule = createShaderModule(ctx.device(), vertShaderCode);
        m_fragmentModule = createShaderModule(ctx.device(), fragShaderCode);

        if (isValid() == false)
        {
            AIKO_ASSERT(false, "Failed to load vulkan shader.");
            unload();
        }

    }

    void VulkanShaderImpl::unload()
    {
        if (m_vertexModule == VK_NULL_HANDLE && m_fragmentModule == VK_NULL_HANDLE)
        {
            return;
        }

        VulkanContext& ctx = VulkanContext::current();
        VkDevice device = ctx.device();

        if (m_vertexModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, m_vertexModule, nullptr);
        }

        if (m_fragmentModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(device, m_fragmentModule, nullptr);
        }

        m_vertexModule = VK_NULL_HANDLE;
        m_fragmentModule = VK_NULL_HANDLE;
        vertex_file.clear();
        fragment_file.clear();
    }


    RenderResourceId VulkanShaderImpl::id() const
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
        return m_vertexModule != VK_NULL_HANDLE && m_fragmentModule != VK_NULL_HANDLE;
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

    VkShaderModule VulkanShaderImpl::createShaderModule(VkDevice device, const vector<uint8_t>& code)
    {

        AIKO_ASSERT(code.empty() == false, "Shader SPIR-V is empty");
        AIKO_ASSERT(code.size() % sizeof(uint32_t) == 0, "Shader SPIR-V size is invalid");

        if (code.empty() || code.size() % sizeof(uint32_t) != 0)
        {
            return VK_NULL_HANDLE;
        }

        const VkShaderModuleCreateInfo info =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(code.data()),
        };
        VkShaderModule module = VK_NULL_HANDLE;
        const VkResult result = vkCreateShaderModule(device, &info, nullptr, &module);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan shader module");
        return module;
    }

}
