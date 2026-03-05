#pragma once

#include "aiko_types.h"
#include "models/component.h"
#include "models/model.h"

namespace aiko
{

    class ModelComponent : public Component
    {
    public:

        ModelComponent();
        virtual ~ModelComponent() = default;

        void load(string);

        Model& getModel();
    private:

        Model m_model;
    };

}