#include "vulkan_computeshader_impl.h"

#include "platform/vulkan/vulkan_context.h"

#include <cstring>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/vulkan/vulkan_platform_helper.h"
#include "platform/vulkan/vulkan_types.h"

namespace aiko::renderer::vulkan
{
    VulkanComputeShaderImpl::VulkanComputeShaderImpl()
    {
    }

    VulkanComputeShaderImpl::~VulkanComputeShaderImpl()
    {
        AIKO_ASSERT(m_module == VK_NULL_HANDLE, "VulkanComputeShaderImpl destroyed without unload()");
    }

    bool VulkanComputeShaderImpl::isValid() const
    {
        return m_module != VK_NULL_HANDLE;
    }

    void VulkanComputeShaderImpl::load(string file)
    {
        unload();

        VulkanContext& ctx = VulkanContext::current();

        auto replacePrefix = [](const char* file, const char* ext) -> string
        {
            std::filesystem::path path(file);
            path.replace_extension(ext);
            return path.generic_string();
        };

        using stdpath = std::filesystem::path;
        const stdpath base = stdpath(global::GLOBAL_PATH) / "build/assets/shaders/vulkan";
        const stdpath computeShaderPath = base / (replacePrefix(file.c_str(), ".comp") + std::string(".spv"));

        const auto code = files::readFileBytes(computeShaderPath.c_str());

        AIKO_ASSERT(code.empty() == false, "Failed to read compute shader file");
        AIKO_ASSERT(code.size() % sizeof(uint32_t) == 0, "Invalid SPIR-V byte size");

        const VkShaderModuleCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(code.data()),
        };

        const VkResult result = vkCreateShaderModule(ctx.device(), &createInfo, nullptr, &m_module);

        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute shader module");

        m_file = file;

    }

    void VulkanComputeShaderImpl::unload()
    {
        if (m_module == VK_NULL_HANDLE)
        {
            m_file.clear();
            return;
        }

        VulkanContext& ctx = VulkanContext::current();

        vkDestroyShaderModule( ctx.device(), m_module, nullptr);

        m_module = VK_NULL_HANDLE;
        m_file.clear();
    }

    uint VulkanComputeShaderImpl::id()
    {
        return static_cast<uint>(reinterpret_cast<uintptr_t>(m_module));
    }


}
