#pragma once

#include <aiko_includes.h>

namespace editor
{
    namespace component
    {

        const void drawComponent(aiko::Component*);

        const void drawTransform(aiko::Transform*);
        const void drawTexture(aiko::TextureComponent*);
        const void drawPboTexture(aiko::PboTextureComponent*);
        const void drawMesh(aiko::MeshComponent*);

    }
}
