#include "components_render.h"

#include <assert.h>

#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_utility.hpp>

#include <imgui.h>

#include "core/imgui_helper.h"

namespace aiko::editor
{
    namespace component
    {

        constexpr const float IMGUI_VELOCITY = .25f;

        enum class ComponentsTypes
        {
            Tranform,
            Camera,
            GridX,
            Light,
            Mesh,
            PboTexture,
            Texture,
            Automaton,
        };

        std::vector<aiko::string> getComponents(aiko::GameObject* obj)
        {
            ComponentsTypes type;
            std::vector<aiko::string> tmp;

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
                if (isComponent<aiko::GridXComponent>(tmp, pmt)) { addCmp(ComponentsTypes::GridX); continue; };
                if (isComponent<aiko::LightComponent>(tmp, pmt)) { addCmp(ComponentsTypes::Light); continue; };;
                if (isComponent<aiko::ca::CellularAutomatonComponent>(tmp, pmt)) { addCmp(ComponentsTypes::Automaton); continue; };
                assert(false && "ERROR :: Component is not supported by the editor");
            }
            return tmp;
        }

        std::vector<aiko::string> getMissingComponents(aiko::GameObject* obj)
        {
            std::vector<aiko::string> componentes = getComponents(obj);
            std::vector<aiko::string> result;

            magic_enum::enum_for_each<ComponentsTypes>([&](auto val)
                {
                    aiko::string str = aiko::string(magic_enum::enum_name<ComponentsTypes>(val));
                    auto found = std::find(componentes.begin(), componentes.end(), str );
                    if (found == componentes.end())
                    {
                        result.push_back(aiko::string(str));
                    }
                });

            return result;
        }

        void removeComponent(aiko::string name, aiko::GameObject* obj)
        {
            auto component = magic_enum::enum_cast<component::ComponentsTypes>(name);
            if (component.has_value() == true)
            {
                switch (component.value())
                {
                case ComponentsTypes::Camera:
                    obj->removeComponent<::aiko::CameraComponent>();
                    break;
                case ComponentsTypes::GridX:
                    obj->removeComponent<::aiko::GridXComponent>();
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

        std::vector<aiko::string> getAllComponents()
        {
            std::array strs = magic_enum::enum_names<ComponentsTypes>();
            return std::vector<aiko::string>(strs.begin(), strs.end());
        }

        void addComponent(aiko::string name, aiko::GameObject* obj)
        {
            auto component = magic_enum::enum_cast<component::ComponentsTypes>(name);
            if ( component.has_value() == true )
            {
                switch (component.value())
                {
                case ComponentsTypes::Camera:
                    obj->addComponent<::aiko::CameraComponent>();
                    break;
                case ComponentsTypes::GridX:
                    obj->addComponent<::aiko::GridXComponent>();
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
                case ComponentsTypes::Automaton:
                    obj->addComponent<::aiko::ca::CellularAutomatonComponent>();
                    break;
                default:
                    assert(false);
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
            if (isComponent<aiko::GridXComponent>(compt, drawGrid)) return;
            if (isComponent<aiko::CameraComponent>(compt, drawCamera)) return;
            if (isComponent<aiko::ca::CellularAutomatonComponent>(compt, drawAutomaton)) return;
            assert(false && "ERROR :: Component is not supported by the editor");
        }

        void drawTransform(aiko::Transform* t)
        {
            ImGui::PushID(t);
            ImGui::DragFloat3("Position", t->position, IMGUI_VELOCITY);
            ImGui::DragFloat3("Rotation", t->rotation, IMGUI_VELOCITY);
            ImGui::DragFloat3("Scale", t->scale, IMGUI_VELOCITY);
            ImGui::PopID();
        }

        void drawTexture(aiko::TextureComponent* text)
        {
            ImGui::PushID(text);
            aiko::texture::Texture texture = text->getTexture()->m_texture;
            imgui::Image(texture);
            ImGui::PopID();
        }

        void drawPboTexture(aiko::PboTextureComponent* pbo)
        {
            ImGui::PushID(pbo);
            ImGui::Checkbox("Auto Render", &pbo->auto_render);
            aiko::texture::PboTexture text = pbo->getPboTexture();
            imgui::Image(text.texture);
            ImGui::PopID();
        }

        void drawMesh(aiko::MeshComponent* mesh)
        {
            ImGui::PushID(mesh);
            ImGui::PopID();
        }

        void drawLight(aiko::LightComponent* light)
        {
            ImGui::PushID(light);
            ImGui::PopID();
        }

        void drawGrid(aiko::GridXComponent* grid)
        {
            ImGui::PushID(grid);
            ImGui::PopID();
        }

        void drawCamera(aiko::CameraComponent* camera)
        {
            ImGui::PushID(camera);
            ImGui::DragFloat3("Position", camera->getCamera()->position, IMGUI_VELOCITY);
            ImGui::DragFloat3("Target", camera->getCamera()->target, IMGUI_VELOCITY);
            ImGui::Spacing();
            ImGui::DragFloat("Near", &camera->getCamera()->m_near, IMGUI_VELOCITY);
            ImGui::DragFloat("Far", &camera->getCamera()->m_far, IMGUI_VELOCITY);
            ImGui::Spacing();

            if (ImGui::BeginCombo("##comboType", magic_enum::enum_name(camera->getCameraType()).data())) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < magic_enum::enum_count<aiko::camera::CameraType>(); n++)
                {
                    aiko::camera::CameraType current = magic_enum::enum_cast<aiko::camera::CameraType>(n).value();
                    bool is_selected = camera->getCameraType() == current; // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), is_selected))
                    {
                        camera->setCameraType(current);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                }
                ImGui::EndCombo();
            }

            if (camera->getCameraType() == camera::CameraType::Orthographic)
            {
                ImGui::DragFloat("OrthoHeight", &camera->getCamera()->m_orthoHeight, IMGUI_VELOCITY);
            }

            ImGui::Spacing();

            if (ImGui::BeginCombo("##comboController", magic_enum::enum_name(camera->getCameraController()).data())) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < magic_enum::enum_count<aiko::camera::CameraController>(); n++)
                {
                    aiko::camera::CameraController current = magic_enum::enum_cast<aiko::camera::CameraController>(n).value();
                    bool is_selected = camera->getCameraType() == current; // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), is_selected))
                    {
                        camera->setCameraController(current);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                }
                ImGui::EndCombo();
            }

            switch (camera->getCameraController())
            {
            case aiko::camera::CameraController::Orbit:
                ImGui::Text("Orbit");
                ImGui::DragFloat("Radius", &camera->radius(), IMGUI_VELOCITY);
                break;
            case aiko::camera::CameraController::Fly:
                ImGui::Text("Fly");
                ImGui::DragFloat("Radius", &camera->speed(), IMGUI_VELOCITY);
                break;
            }

            ImGui::PopID();
        }

        void drawAutomaton(aiko::ca::CellularAutomatonComponent* cmp)
        {
            ImGui::PushID(cmp);
            ImGui::Text("CellularAutomatonComponent");
            if (ImGui::Button("Regenerate"))
            {
                cmp->getWorld().regenerate();
            }
            ImGui::PopID();
        }

    }
}