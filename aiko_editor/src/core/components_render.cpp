#include "components_render.h"

#include <assert.h>

#include <imgui.h>
#include <magic_enum_all.hpp>

namespace editor
{
    namespace component
    {

        enum class ComponentsTypes
        {
            Tranform,
            Camera,
            Grid,
            Light,
            Mesh,
            PboTexture,
            Texture,
        };

        std::vector<std::string> getComponents(aiko::GameObject* obj)
        {
            ComponentsTypes type;
            std::vector<std::string> tmp;

            auto addCmp = [&](ComponentsTypes type)
                {
                    tmp.push_back(magic_enum::enum_name(type).data());
                };

            for (auto* tmp : obj->getComponents())
            {
                constexpr auto pmt = [](auto*) {};
                if (isComponent<aiko::Transform>(tmp, pmt)) { addCmp(ComponentsTypes::Tranform); continue; };
                if (isComponent<aiko::TextureComponent>(tmp, pmt)) { addCmp(ComponentsTypes::Texture); continue; };
                if (isComponent<aiko::PboTextureComponent>(tmp, pmt)) { addCmp(ComponentsTypes::PboTexture); continue; };
                if (isComponent<aiko::MeshComponent>(tmp, pmt)) { addCmp(ComponentsTypes::Mesh); continue; };
                assert(false && "ERROR :: Component is not supported by the editor");
            }
            return tmp;
        }

        std::vector<std::string> getMissingComponents(aiko::GameObject* obj)
        {
            std::vector<std::string> componentes = getComponents(obj);
            std::vector<std::string> result;

            magic_enum::enum_for_each<ComponentsTypes>([&](auto val)
                {
                    std::string str = std::string(magic_enum::enum_name<ComponentsTypes>(val));
                    auto found = std::find(componentes.begin(), componentes.end(), str );
                    if (found != componentes.end())
                    {
                        // Already exist
                    }
                    else
                    {
                        result.push_back(std::string(str));
                    }
                });

            return result;
        }

        void removeComponent(std::string name, aiko::GameObject* obj)
        {
            auto component = magic_enum::enum_cast<::editor::component::ComponentsTypes>(name);
            if (component.has_value() == true)
            {
                switch (component.value())
                {
                case ComponentsTypes::Camera:
                    obj->removeComponent<::aiko::CameraComponent>();
                    break;
                case ComponentsTypes::Grid:
                    obj->removeComponent<::aiko::GridComponent>();
                    break;
                case ComponentsTypes::Light:
                    obj->removeComponent<::aiko::LightComponent>();
                    break;
                case ComponentsTypes::Mesh:
                    obj->removeComponent<::aiko::MeshComponent>();
                    break;
                case ComponentsTypes::PboTexture:
                    obj->removeComponent<::aiko::PboTextureComponent>();
                    break;
                case ComponentsTypes::Texture:
                    obj->removeComponent<::aiko::TextureComponent>();
                    break;
                default:
                    break;
                }
            }
        }

        std::vector<std::string> getAllComponents()
        {
            std::array strs = magic_enum::enum_names<ComponentsTypes>();
            return std::vector<std::string>(strs.begin(), strs.end());
        }

        void addComponent(std::string name, aiko::GameObject* obj)
        {
            auto component = magic_enum::enum_cast<::editor::component::ComponentsTypes>(name);
            if ( component.has_value() == true )
            {
                switch (component.value())
                {
                case ComponentsTypes::Camera:
                    obj->addComponent<::aiko::CameraComponent>();
                    break;
                case ComponentsTypes::Grid:
                    obj->addComponent<::aiko::GridComponent>();
                    break;
                case ComponentsTypes::Light:
                    obj->addComponent<::aiko::LightComponent>();
                    break;
                case ComponentsTypes::Mesh:
                    obj->addComponent<::aiko::MeshComponent>();
                    break;
                case ComponentsTypes::PboTexture:
                    obj->addComponent<::aiko::PboTextureComponent>();
                    break;
                case ComponentsTypes::Texture:
                    obj->addComponent<::aiko::TextureComponent>();
                    break;
                default:
                    break;
                }
            }
        }

        void drawComponent(aiko::Component* compt)
        {
            if (isComponent<aiko::Transform>(compt, drawTransform)) return;
            if (isComponent<aiko::TextureComponent>(compt, drawTexture)) return;
            if (isComponent<aiko::PboTextureComponent>(compt, drawPboTexture)) return;
            if (isComponent<aiko::MeshComponent>(compt, drawMesh)) return;
            if (isComponent<aiko::LightComponent>(compt, drawLight)) return;
            assert(false && "ERROR :: Component is not supported by the editor");
        }

        void drawTransform(aiko::Transform* t)
        {
            ImGui::DragFloat3("Position", t->position);
            ImGui::DragFloat3("Rotation", t->rotation);
            ImGui::DragFloat3("Scale", t->scale);

        }

        void drawTexture(aiko::TextureComponent* text)
        {

            aiko::texture::Texture texture = text->getTexture();
            // Using a Child allow to fill all the space of the window.
            // It also allows customization
            ImGui::BeginChild("Texture");
            // Get the size of the child (i.e. the whole draw size of the windows).

            // Get the dimensions of the texture
            ImVec2 textureSize = ImVec2(texture.width, texture.height);

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - textureSize.x) * 0.5f, 0));
            ImGui::Image((ImTextureID)texture.id, ImVec2(textureSize.x, textureSize.y), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::EndChild();

        }

        void drawPboTexture(aiko::PboTextureComponent* pbo)
        {
            ImGui::Checkbox("Auto Render", &pbo->auto_render);

            ImGui::BeginChild("Texture");
            // Get the size of the child (i.e. the whole draw size of the windows).

            aiko::texture::PboTexture text = pbo->getPboTexture();

            // Get the dimensions of the texture
            ImVec2 textureSize = ImVec2(text.texture.width, text.texture.height);

            ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - textureSize.x) * 0.5f, 0));
            ImGui::Image((ImTextureID)text.texture.id, ImVec2(textureSize.x, textureSize.y), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::EndChild();
            
        }

        void drawMesh(aiko::MeshComponent* mesh)
        {

        }

        void drawLight(aiko::LightComponent*)
        {

        }

    }
}