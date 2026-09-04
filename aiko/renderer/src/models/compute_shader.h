 #pragma once

#include <aiko_types.h>
#include "interfaces/icomputeshader_impl.h"

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
        class IRenderDevice;
    }

    class ComputeShader
    {
        friend class renderer::RendererFactory;
        friend class renderer::IRenderDevice;
    public:

        // Copy
        ComputeShader(const ComputeShader&) = delete;
        ComputeShader& operator=(const ComputeShader&) = delete;

        // Move
        ComputeShader(ComputeShader&&) noexcept = delete;
        ComputeShader& operator=(ComputeShader&&) noexcept = delete;

        ComputeShader();
        ~ComputeShader();

        bool isValid() const;

        // load
        void load(string file);
        void unload();

        RenderResourceId id() const;

    private:

        AikoPtr<interfaces::IComputeShaderImpl> backend;

    };

}
