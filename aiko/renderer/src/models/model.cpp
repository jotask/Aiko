#include "model.h"

namespace aiko
{

    AssetSystem* Model::s_assetSystem = nullptr;

    Model::Model()
    {

    }

    void Model::load(const char* filename)
    {
        AIKO_NOT_IMPLEMENTED;
        /*
        m_meshes = s_assetSystem->loadModel(filename);
        for (auto& mesh : m_meshes)
        {
            mesh.material.m_shader.load("model");
        }
        */
    }

}
