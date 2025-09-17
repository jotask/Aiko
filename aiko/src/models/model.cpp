#include "model.h"

#include <assert.h>

#include "systems/asset_system.h"

namespace aiko
{

    AssetSystem* Model::s_assetSystem = nullptr;

    Model::Model()
    {

    }

    void Model::load(const char* filename)
    {
        // FIXME
		auto model = s_assetSystem->loadModel(filename);
        this->m_mesh = model.m_mesh;
        this->m_material = model.m_material;
        this->m_material.m_shader.load("aiko");
        assert(this->m_material.m_shader.isvalid() && "Shader is invalid");
        this->m_material.m_shader.preLoadUniforms({ {"u_texture", ShaderUniformDataType::SHADER_UNIFORM_SAMPLER2D} });
    }

}
