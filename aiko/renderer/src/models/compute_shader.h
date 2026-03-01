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
        ComputeShader(const ComputeShader&) = default;
        ComputeShader& operator=(const ComputeShader&) = default;

        // Move
        ComputeShader(ComputeShader&&) noexcept = default;
        ComputeShader& operator=(ComputeShader&&) noexcept = default;

        ComputeShader();
        virtual ~ComputeShader() = default;

        void* getImpl() const { return backend.get(); }

        bool isValid() const;

        // load
        void load(string file);
        void unload();

        uint id();

    private:

        AikoPtr<interfaces::IComputeShaderImpl> backend;

    };

}
