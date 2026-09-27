#include "components_render.h"

#include <algorithm>

#include "registry/component_registry.h"
#include "core/imgui_helper.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

namespace aiko::editor
{
    namespace component
    {

        constexpr const float IMGUI_VELOCITY = .25f;

        void drawComponent(Component* component)
        {
            AIKO_ASSERT(component != nullptr, "Cannot render null component");

            if (component == nullptr)
            {
                return;
            }

            const ComponentEditorEntry* entry = findComponentEntry(*component);

            AIKO_ASSERT(entry != nullptr, "Component is not supported by the editor");

            if (entry == nullptr)
            {
                return;
            }

            entry->render(*component);
        }

        void drawTransform(TransformComponent* t)
        {
            ImGui::PushID(t);
            ImGui::DragFloat3("Position", &t->transform.position.x, IMGUI_VELOCITY);
            ImGui::DragFloat3("Rotation", &t->transform.rotation.x, IMGUI_VELOCITY);
            ImGui::DragFloat3("Scale", &t->transform.scale.x, IMGUI_VELOCITY);
            ImGui::PopID();
        }

        void drawSprite(aiko::SpriteComponent* sprite)
        {
            ImGui::PushID(sprite);

            vec2 size = sprite->getSize();
            if (ImGui::DragFloat2("Size", &size.x, IMGUI_VELOCITY, 0.01f))
            {
                size.x = std::max(size.x, 0.01f);
                size.y = std::max(size.y, 0.01f);

                sprite->setSize(size);
            }

            vec2 pivot = sprite->getPivot();
            if (ImGui::DragFloat2("Pivot", &pivot.x, IMGUI_VELOCITY, 0.0f, 1.0f))
            {
                pivot.x = std::clamp(pivot.x, 0.0f, 1.0f);
                pivot.y = std::clamp(pivot.y, 0.0f, 1.0f);

                sprite->setPivot(pivot);
            }

            bool flipX = sprite->getFlipX();
            if (ImGui::Checkbox("Flip X", &flipX))
            {
                sprite->setFlipX(flipX);
            }

            bool flipY = sprite->getFlipY();
            if (ImGui::Checkbox("Flip Y", &flipY))
            {
                sprite->setFlipY(flipY);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Texture size: %zu x %zu", sprite->getWidth(), sprite->getHeight());

            ImGui::PopID();
        }

        void drawMesh(MeshComponent* mesh)
        {
            ImGui::PushID(mesh);

            Material& material = mesh->getMaterial();

            float color[4] =
            {
                material.m_baseColor.r,
                material.m_baseColor.g,
                material.m_baseColor.b,
                material.m_baseColor.a
            };

            if (ImGui::ColorEdit4("Base Color", color))
            {
                material.m_baseColor =
                {
                    color[0],
                    color[1],
                    color[2],
                    color[3]
                };
            }

            ImGui::Checkbox("Lit", &material.m_lit);

            ImGui::Checkbox("Use Vertex Color", &material.m_useVertexColor);

            ImGui::PopID();
        }

        void drawLight(LightComponent* light)
        {
            ImGui::PushID(light);

            if (ImGui::BeginCombo("Type", magic_enum::enum_name(light->type).data()))
            {
                for (int i = 0; i < magic_enum::enum_count<LightType>(); ++i)
                {
                    const LightType current = magic_enum::enum_cast<LightType>(i).value();

                    const bool selected = light->type == current;

                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), selected))
                    {
                        light->type = current;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            float color[4] =
            {
                light->color.r,
                light->color.g,
                light->color.b,
                light->color.a
            };

            if (ImGui::ColorEdit4("Color", color))
            {
                light->color =
                {
                    color[0],
                    color[1],
                    color[2],
                    color[3]
                };
            }

            ImGui::DragFloat("Intensity", &light->intensity, IMGUI_VELOCITY, 0.0f);

            switch (light->type)
            {
            case LightType::Directional:
                ImGui::DragFloat3("Direction", &light->direction.x, IMGUI_VELOCITY);
                break;

            case LightType::Point:
                ImGui::DragFloat("Range", &light->range, IMGUI_VELOCITY, 0.0f);
                break;

            case LightType::Spot:
                ImGui::DragFloat3("Direction", &light->direction.x, IMGUI_VELOCITY);
                ImGui::DragFloat("Range", &light->range, IMGUI_VELOCITY, 0.0f);
                ImGui::DragFloat("Inner Cos", &light->innerCos, 0.01f, -1.0f, 1.0f);
                ImGui::DragFloat("Outer Cos", &light->outerCos, 0.01f, -1.0f, 1.0f);
                break;
            }

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

            if (ImGui::BeginCombo("##comboType", magic_enum::enum_name(camera->getCameraType()).data()))
            {
                for (int n = 0; n < magic_enum::enum_count<Camera::CameraType>(); ++n)
                {
                    Camera::CameraType current = magic_enum::enum_cast<Camera::CameraType>(n).value();
                    bool is_selected = camera->getCameraType() == current;
                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), is_selected))
                    {
                        camera->setCameraType(current);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
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
                    bool is_selected = camera->getCameraController() == current;
                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), is_selected))
                    {
                        camera->setCameraController(current);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
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
                ImGui::DragFloat("Speed", &camera->speed(), IMGUI_VELOCITY);
                break;
            }

            ImGui::PopID();
        }

    }
}