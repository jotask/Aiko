#include "vulkan_computeshader_impl.h"

#include "platform/vulkan/vulkan_context.h"

#include <cstring>
#include <atomic>
#include <cstdlib>

#include <logger/logger.h>
#include <core/file.h>

#include "platform/vulkan/vulkan_platform_helper.h"
#include "platform/vulkan/vulkan_types.h"
#include "platform/vulkan/vulkan_shader_reflector.h"

namespace aiko::renderer::vulkan
{

    namespace
    {
        RenderResourceId nextComputeShaderId()
        {
            static std::atomic<RenderResourceId> nextId{1};
            const RenderResourceId id = nextId.fetch_add(1, std::memory_order_relaxed);
            if (id == InvalidRenderResourceId)
            {
                std::abort();
            }
            return id;
        }
    }

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
        const stdpath base(AIKO_VULKAN_SHADER_BINARY_DIR);
        const stdpath computeShaderPath = base / (replacePrefix(file.c_str(), ".comp") + std::string(".spv"));

        const auto code = files::readFileBytes(computeShaderPath);
        AIKO_ASSERT(code.empty() == false, "Failed to read compute shader file");
        AIKO_ASSERT(code.size() % sizeof(uint32_t) == 0, "Invalid SPIR-V byte size");

        m_reflection.clear();

        reflectShaderSpirv(code, VK_SHADER_STAGE_COMPUTE_BIT, m_reflection);
        validateComputeShaderAbi(m_reflection);

        const VkShaderModuleCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = reinterpret_cast<const uint32_t*>(code.data()),
        };

        const VkResult result = vkCreateShaderModule(ctx.device(), &createInfo, nullptr, &m_module);
        AIKO_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan compute shader module");

        if (result != VK_SUCCESS)
        {
            m_module = VK_NULL_HANDLE;
            m_reflection.clear();
            return;
        }

        m_id = nextComputeShaderId();
        m_file = file;

    }

    void VulkanComputeShaderImpl::unload()
    {

        m_id = InvalidRenderResourceId;

        if (m_module == VK_NULL_HANDLE)
        {
            m_file.clear();
            m_reflection.clear();
            return;
        }

        VulkanContext& ctx = VulkanContext::current();

        vkDestroyShaderModule( ctx.device(), m_module, nullptr);

        m_module = VK_NULL_HANDLE;
        m_file.clear();
        m_reflection.clear();
    }

    RenderResourceId VulkanComputeShaderImpl::id() const
    {
        return m_id;
    }


}
