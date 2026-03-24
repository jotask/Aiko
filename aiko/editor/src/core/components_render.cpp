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

        static const std::vector<ComponentEditorEntry> s_componentEntries =
        {{
            {
                "Transform",
                [](aiko::GameObject* go) { return go->getComponent<aiko::TransforComponent>() != nullptr; },
                [](aiko::GameObject* go) { go->addComponent<aiko::TransforComponent>(); },
                [](aiko::GameObject* go) { go->removeComponent<aiko::TransforComponent>(); },
                [](aiko::Component* c) -> bool
                {
                    if (auto* t = dynamic_cast<aiko::TransforComponent*>(c))
                    {
                        drawTransform(t);
                        return true;
                    }
                    return false;
                }
            },
            {
                "Camera",
                [](aiko::GameObject* go) { return go->getComponent<aiko::CameraComponent>() != nullptr; },
                [](aiko::GameObject* go) { go->addComponent<aiko::CameraComponent>(); },
                [](aiko::GameObject* go) { go->removeComponent<aiko::CameraComponent>(); },
                [](aiko::Component* c) -> bool
                {
                    if (auto* t = dynamic_cast<aiko::CameraComponent*>(c))
                    {
                        drawCamera(t);
                        return true;
                    }
                    return false;
                }
            },
            {
                "Light",
                [](aiko::GameObject* go) { return go->getComponent<aiko::LightComponent>() != nullptr; },
                [](aiko::GameObject* go) { go->addComponent<aiko::LightComponent>(); },
                [](aiko::GameObject* go) { go->removeComponent<aiko::LightComponent>(); },
                [](aiko::Component* c) -> bool
                {
                    if (auto* t = dynamic_cast<aiko::LightComponent*>(c))
                    {
                        drawLight(t);
                        return true;
                    }
                    return false;
                }
            },
            {
                "Mesh",
                [](aiko::GameObject* go) { return go->getComponent<aiko::MeshComponent>() != nullptr; },
                [](aiko::GameObject* go) { go->addComponent<aiko::MeshComponent>(); },
                [](aiko::GameObject* go) { go->removeComponent<aiko::MeshComponent>(); },
                [](aiko::Component* c) -> bool
                {
                    if (auto* t = dynamic_cast<aiko::MeshComponent*>(c))
                    {
                        drawMesh(t);
                        return true;
                    }
                    return false;
                }
            },
            {
                "Sprite",
                [](aiko::GameObject* go) { return go->getComponent<aiko::SpriteComponent>() != nullptr; },
                [](aiko::GameObject* go) { go->addComponent<aiko::SpriteComponent>(); },
                [](aiko::GameObject* go) { go->removeComponent<aiko::SpriteComponent>(); },
                [](aiko::Component* c) -> bool
                {
                    if (auto* t = dynamic_cast<aiko::SpriteComponent*>(c))
                    {
                        drawSprite(t);
                        return true;
                    }
                    return false;
                }
            }
        }};

        constexpr const float IMGUI_VELOCITY = .25f;

        std::vector<string> getMissingComponents(GameObject* obj)
        {
            std::vector<string> result;
            for (const auto& entry : s_componentEntries)
            {
                if (entry.has(obj) == false)
                {
                    result.push_back(entry.name);
                }
            }
            return result;
        }

        void removeComponent(string name, GameObject* obj)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.name == name)
                {
                    entry.remove(obj);
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

        void addComponent(string name, GameObject* obj)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.name == name)
                {
                    entry.add(obj);
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

        void drawComponent(Component* compt)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.draw(compt) == true)
                {
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

        void drawTransform(aiko::TransforComponent* t)
        {
            ImGui::PushID(t);
            ImGui::DragFloat3("Position", t->transform.position, IMGUI_VELOCITY);
            ImGui::DragFloat3("Rotation", t->transform.rotation, IMGUI_VELOCITY);
            ImGui::DragFloat3("Scale", t->transform.scale, IMGUI_VELOCITY);
            ImGui::PopID();
        }

        void drawSprite(aiko::SpriteComponent* text)
        {
            AIKO_NOT_IMPLEMENTED;
            /*
            ImGui::PushID(text);
            Texture texture = text->getTexture();
            imgui::Image(texture);
            ImGui::PopID();
            */
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

        void drawCamera(aiko::CameraComponent* camera)
        {
            ImGui::PushID(camera);
            ImGui::DragFloat3("Position", camera->getCamera().position, IMGUI_VELOCITY);
            ImGui::DragFloat3("Target", camera->getCamera().target, IMGUI_VELOCITY);
            ImGui::Spacing();
            ImGui::DragFloat("Near", &camera->getCamera().m_near, IMGUI_VELOCITY);
            ImGui::DragFloat("Far", &camera->getCamera().m_far, IMGUI_VELOCITY);
            ImGui::Spacing();

            if (ImGui::BeginCombo("##comboType", magic_enum::enum_name(camera->getCameraType()).data())) // The second parameter is the label previewed before opening the combo.
            {
                for (int n = 0; n < magic_enum::enum_count<Camera::CameraType>(); n++)
                {
                    Camera::CameraType current = magic_enum::enum_cast<Camera::CameraType>(n).value();
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

            if (camera->getCameraType() == Camera::CameraType::Orthographic)
            {
                ImGui::DragFloat("OrthoHeight", &camera->getCamera().m_orthoHeight, IMGUI_VELOCITY);
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

    }
}