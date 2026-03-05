#include "model_component.h"

#include "constants.h"
#include "models/game_object.h"
#include "systems/render_system.h"

namespace aiko
{

    ModelComponent::ModelComponent()
        : Component("Model")
    {

    }

    void ModelComponent::load(string filename)
    {
        m_model.load(filename.c_str());
    }

    Model& ModelComponent::getModel()
    {
        return m_model;
    }
}
