#include "compute_shader.h"

#include "render_factory.h"

namespace aiko
{

    ComputeShader::ComputeShader()
        : backend(renderer::RendererFactory::createComputeShaderImpl())
    {

    }

    bool ComputeShader::isValid() const
    {
        return backend->isValid();
    }

    void ComputeShader::load(string file)
    {
        backend->load(file);
    }

    void ComputeShader::unload()
    {
        return backend->unload();
    }

    uint ComputeShader::id()
    {
        return backend->id();
    }
}
