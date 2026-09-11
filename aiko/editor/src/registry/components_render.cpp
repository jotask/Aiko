#include "components_render.h"

#include "registry/component_registry.h"
#include "core/imgui_helper.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

namespace aiko::editor
{
    namespace component
    {

        constexpr const float IMGUI_VELOCITY = .25f;

        void drawComponent(Component* compt)
        {
            for (const auto& entry : s_componentEntries)
            {
                if (entry.render(compt) == true)
                {
                    return;
                }
            }
            AIKO_ASSERT(false, "ERROR :: Component is not supported by the editor");
        }

        void drawTransform(TransforComponent* t)
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

        void drawMesh(MeshComponent* mesh)
        {
            ImGui::PushID(mesh);
            ImGui::PopID();
        }

        void drawLight(LightComponent* light)
        {
            ImGui::PushID(light);
            ImGui::PopID();
        }

        void drawCamera(CameraComponent* camera)
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