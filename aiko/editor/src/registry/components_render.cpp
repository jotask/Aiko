#include "components_render.h"

#include "ImGuiFileDialog.h"
#include "ImGuiFileDialogConfig.h"
#include "components/model_component.h"
#include "constants.h"
#include "core/imgui_helper.h"
#include "registry/material_render.h"
#include "registry/component_registry.h"

#include <magic_enum/magic_enum.hpp>

#include <algorithm>
#include <filesystem>
#include <imgui.h>

namespace aiko::editor
{
    namespace component
    {

        constexpr const float IMGUI_VELOCITY = .25f;

        template<class T>
        bool drawAssetSource(T* component, const char* label, const char* dialogKey, const char* filter)
        {
            AIKO_ASSERT(component != nullptr, "Cannot draw asset source for null component");

            if (component == nullptr)
            {
                return false;
            }

            bool changed = false;

            const string& source = component->getAssetSource();

            ImGui::Text("%s: %s", label, source.empty() ? "<None>" : source.c_str());

            if (ImGui::Button("Select Asset"))
            {
                IGFD::FileDialogConfig config;
                config.path = global::GLOBAL_ASSET_PATH;

                ImGuiFileDialog::Instance()->OpenDialog(dialogKey, "Choose Asset", filter, config);
            }

            if (ImGuiFileDialog::Instance()->Display(dialogKey))
            {
                if (ImGuiFileDialog::Instance()->IsOk())
                {
                    const std::filesystem::path selectedPath = ImGuiFileDialog::Instance()->GetFilePathName();

                    const std::filesystem::path assetRoot = global::GLOBAL_ASSET_PATH;

                    std::error_code error;

                    const std::filesystem::path relativePath = std::filesystem::relative(selectedPath, assetRoot, error);

                    AIKO_ASSERT(error.value() == 0, "Failed to make selected asset path relative");

                    if (error.value() == 0)
                    {
                        const std::filesystem::path normalizedPath = relativePath.lexically_normal();
                        const bool outsideAssetRoot = normalizedPath.empty() == false && *normalizedPath.begin() == "..";
                        AIKO_ASSERT(outsideAssetRoot == false, "Selected file must be inside the asset directory");
                        if (outsideAssetRoot == false)
                        {
                            component->load(normalizedPath.generic_string());
                            changed = true;
                        }
                    }
                }

                ImGuiFileDialog::Instance()->Close();
            }
            return changed;
        }

        bool drawComponent(Component* component)
        {
            AIKO_ASSERT(component != nullptr, "Cannot render null component");

            if (component == nullptr)
            {
                return false;
            }

            const ComponentEditorEntry* entry = findComponentEntry(*component);

            AIKO_ASSERT(entry != nullptr, "Component is not supported by the editor");

            if (entry == nullptr)
            {
                return false;
            }

            return entry->render(*component);
        }

        bool drawTransform(TransformComponent* t)
        {
            bool changed = false;
            ImGui::PushID(t);
            changed |= ImGui::DragFloat3("Position", &t->transform.position.x, IMGUI_VELOCITY);
            changed |= ImGui::DragFloat3("Rotation", &t->transform.rotation.x, IMGUI_VELOCITY);
            changed |= ImGui::DragFloat3("Scale", &t->transform.scale.x, IMGUI_VELOCITY);
            ImGui::PopID();
            return changed;
        }

        bool drawSprite(aiko::SpriteComponent* sprite)
        {
            bool changed = false;

            ImGui::PushID(sprite);

            changed |= drawAssetSource(sprite, "Texture", "SpriteTextureDialog", "Image files{.png,.jpg,.jpeg,.bmp,.tga}");

            ImGui::Spacing();

            vec2 size = sprite->getSize();
            if (ImGui::DragFloat2("Size", &size.x, IMGUI_VELOCITY, 0.01f))
            {
                size.x = std::max(size.x, 0.01f);
                size.y = std::max(size.y, 0.01f);

                sprite->setSize(size);
                changed = true;
            }

            vec2 pivot = sprite->getPivot();
            if (ImGui::DragFloat2("Pivot", &pivot.x, IMGUI_VELOCITY, 0.0f, 1.0f))
            {
                pivot.x = std::clamp(pivot.x, 0.0f, 1.0f);
                pivot.y = std::clamp(pivot.y, 0.0f, 1.0f);

                sprite->setPivot(pivot);
                changed = true;
            }

            bool flipX = sprite->getFlipX();
            if (ImGui::Checkbox("Flip X", &flipX))
            {
                sprite->setFlipX(flipX);
                changed = true;
            }

            bool flipY = sprite->getFlipY();
            if (ImGui::Checkbox("Flip Y", &flipY))
            {
                sprite->setFlipY(flipY);
                changed = true;
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Texture size: %zu x %zu", sprite->getWidth(), sprite->getHeight());

            ImGui::PopID();
            return changed;
        }

        bool drawMesh(MeshComponent* mesh)
        {
            bool changed = false;

            const MeshComponent::MeshPrimitive primitive = mesh->getPrimitive();

            const char* preview = primitive == MeshComponent::MeshPrimitive::None ? "None" : magic_enum::enum_name(primitive).data();

            if (ImGui::BeginCombo("Mesh##Primitive", preview))
            {
                for (const MeshComponent::MeshPrimitive current : magic_enum::enum_values<MeshComponent::MeshPrimitive>())
                {
                    if (current == MeshComponent::MeshPrimitive::None)
                    {
                        continue;
                    }

                    const bool selected = current == primitive;

                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), selected))
                    {
                        mesh->loadPrimitive(current);
                        changed = true;
                    }

                    if (selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::Spacing();
            ImGui::SeparatorText("Material");

            changed |= drawMaterial(mesh->getMaterial());
            return changed;
        }

        bool drawModel(ModelComponent* model)
        {
            bool changed = false;
            ImGui::PushID(model);
            changed |= drawAssetSource(model, "Model", "ModelAssetDialog", "Model files{.obj,.fbx,.gltf,.glb}");
            ImGui::PopID();
            return changed;
        }

        bool drawLight(LightComponent* light)
        {
            bool changed = false;

            ImGui::PushID(light);

            ImGui::Spacing();

            if (ImGui::BeginCombo("Type", magic_enum::enum_name(light->type).data()))
            {
                for (int i = 0; i < magic_enum::enum_count<LightType>(); ++i)
                {
                    const LightType current = magic_enum::enum_cast<LightType>(i).value();

                    const bool selected = light->type == current;

                    if (ImGui::Selectable(magic_enum::enum_name(current).data(), selected))
                    {
                        light->type = current;
                        changed = true;
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
                changed = true;
            }

            changed |= ImGui::DragFloat("Intensity", &light->intensity, IMGUI_VELOCITY, 0.0f);

            switch (light->type)
            {
            case LightType::Directional:
                changed |= ImGui::DragFloat3("Direction", &light->direction.x, IMGUI_VELOCITY);
                break;

            case LightType::Point:
                changed |= ImGui::DragFloat("Range", &light->range, IMGUI_VELOCITY, 0.0f);
                break;

            case LightType::Spot:
                changed |= ImGui::DragFloat3("Direction", &light->direction.x, IMGUI_VELOCITY);
                changed |= ImGui::DragFloat("Range", &light->range, IMGUI_VELOCITY, 0.0f);
                changed |= ImGui::DragFloat("Inner Cos", &light->innerCos, 0.01f, -1.0f, 1.0f);
                changed |= ImGui::DragFloat("Outer Cos", &light->outerCos, 0.01f, -1.0f, 1.0f);
                break;
            }

            ImGui::PopID();
            return changed;
        }

        bool drawCamera(CameraComponent* camera)
        {
            bool changed = false;
            ImGui::PushID(camera);
            changed |= ImGui::DragFloat3("Position", camera->getCamera().position, IMGUI_VELOCITY);
            changed |= ImGui::DragFloat3("Target", camera->getCamera().target, IMGUI_VELOCITY);
            ImGui::Spacing();
            changed |= ImGui::DragFloat("Near", &camera->getCamera().m_near, IMGUI_VELOCITY);
            changed |= ImGui::DragFloat("Far", &camera->getCamera().m_far, IMGUI_VELOCITY);
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
                        changed = true;
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
                changed |= ImGui::DragFloat("OrthoHeight", &camera->getCamera().m_orthoHeight, IMGUI_VELOCITY);
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
                        changed = true;
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
                changed |= ImGui::DragFloat("Radius", &camera->radius(), IMGUI_VELOCITY);
                break;
            case aiko::camera::CameraController::Fly:
                ImGui::Text("Fly");
                changed |= ImGui::DragFloat("Speed", &camera->speed(), IMGUI_VELOCITY);
                break;
            }

            ImGui::PopID();
            return changed;
        }

    }
}