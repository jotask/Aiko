#pragma once

#include <aiko_includes.h>
#include <vector>

namespace aiko::editor
{
    namespace component
    {

        struct ComponentEditorEntry
        {
            string name;
            bool (*has)(GameObject*);
            void (*add)(GameObject*);
            void (*remove)(GameObject*);
            bool (*draw)(Component*);
        };

        std::vector<string> getMissingComponents(aiko::GameObject*);
        void addComponent(aiko::string, aiko::GameObject*);
        void removeComponent(aiko::string, aiko::GameObject*);

        void drawComponent(aiko::Component*);

        void drawTransform(aiko::TransforComponent*);
        void drawSprite(aiko::SpriteComponent*);
        void drawMesh(aiko::MeshComponent*);
        void drawLight(aiko::LightComponent*);
        void drawCamera(aiko::CameraComponent*);

    }
}
