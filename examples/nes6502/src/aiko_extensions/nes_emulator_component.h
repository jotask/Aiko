#pragma once

#include "aiko_types.h"
#include "components/pbo_texture_component.h"
#include "models/component.h"
#include "models/mesh.h"
#include "models/shader.h"

namespace nes
{

    class NesComponent : public aiko::PboTextureComponent
    {
    public:

        NesComponent();
        virtual ~NesComponent() = default;

        virtual void init() override;
        virtual void update() override;
        virtual void render() override;

    private:

    };

}