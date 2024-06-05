#pragma once

#include <aiko_includes.h>
#include <vector>

namespace aiko::editor
{
    namespace component
    {

        std::vector<aiko::string> getAllComponents();
        std::vector<aiko::string> getComponents(aiko::GameObject*);
        std::vector<aiko::string> getMissingComponents(aiko::GameObject*);
        void addComponent(aiko::string, aiko::GameObject*);
        void removeComponent(aiko::string, aiko::GameObject*);

        void drawComponent(aiko::Component*);

        void drawTransform(aiko::Transform*);
        void drawTexture(aiko::TextureComponent*);
        void drawPboTexture(aiko::PboTextureComponent*);
        void drawMesh(aiko::MeshComponent*);
        void drawLight(aiko::LightComponent*);
        void drawGrid(aiko::GridComponent*);
        void drawGrid(aiko::GridComponent*);
        void drawCamera(aiko::CameraComponent*);

        template <typename T>
        bool isComponent(aiko::Component* compt, void (*fnt)(T*))
        {
            if (T* derived = dynamic_cast<T*>(compt))
            {
                fnt(derived);
                return true;
            }
            return false;
        }


    }
}
