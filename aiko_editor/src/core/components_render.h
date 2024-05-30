#pragma once

#include <aiko_includes.h>
#include <vector>

namespace editor
{
    namespace component
    {

        std::vector<std::string> getAllComponents();
        std::vector<std::string> getComponents(aiko::GameObject*);
        std::vector<std::string> getMissingComponents(aiko::GameObject*);
        void addComponent(std::string, aiko::GameObject*);
        void removeComponent(std::string, aiko::GameObject*);

        void drawComponent(aiko::Component*);

        void drawTransform(aiko::Transform*);
        void drawTexture(aiko::TextureComponent*);
        void drawPboTexture(aiko::PboTextureComponent*);
        void drawMesh(aiko::MeshComponent*);
        void drawLight(aiko::LightComponent*);

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
