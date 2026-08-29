 #pragma once

#include <aiko_types.h>
#include "interfaces/icomputeshader_impl.h"

 namespace aiko
{
    namespace renderer
    {
        class RendererFactory;
    }

    class ComputeShader
    {
    public:

        friend class renderer::RendererFactory;

        // Copy
        ComputeShader(const ComputeShader&) = delete;
        ComputeShader& operator=(const ComputeShader&) = delete;

        // Move
        ComputeShader(ComputeShader&&) noexcept = default;
        ComputeShader& operator=(ComputeShader&&) noexcept = default;

        ComputeShader();
        ~ComputeShader() = default;

        void* getImpl() const { return backend.get(); }

        bool isValid() const;

        // load
        void load(string file);
        void unload();

        RenderResourceId id() const;

    private:

        AikoPtr<interfaces::IComputeShaderImpl> backend;

    };

}
