#include "scene_view_panel.h"

#include "camera/camera_controller.h"
#include "commands/transform/transform_command.h"
#include "core/editor_context.h"
#include "models/game_object.h"
#include "systems/render_system.h"

#include <math/math.h>

#include <imgui.h>
#include <ImGuizmo.h>

namespace aiko::editor
{

    SceneViewPanel::SceneViewPanel()
        : EditorPanel("Scene")
    {
        m_camera.setCameraType(Camera::CameraType::Perspective);

        m_camera.position =
        {
            0.0f,
            2.5f,
            8.0f
        };

        m_camera.target =
        {
            0.0f,
            0.0f,
            0.0f
        };

        m_renderTarget.create(1280, 720);
    }

    void SceneViewPanel::render(EditorContext& context)
    {
        if (ImGui::Begin("Scene"))
        {
            RenderSystem& renderSystem = context.renderSystem();

            const ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            float imageWidth = availableSpace.x;
            float imageHeight = availableSpace.y;

            if (imageWidth > 0.0f && imageHeight > 0.0f)
            {
                const u32 targetWidth = static_cast<u32>(imageWidth);

                const u32 targetHeight = static_cast<u32>(imageHeight);

                const ivec2 currentSize = m_renderTarget.size();

                if (currentSize.x != static_cast<int>(targetWidth) || currentSize.y != static_cast<int>(targetHeight))
                {
                    m_renderTarget.resize(targetWidth, targetHeight);
                }

                renderSystem.renderToTarget(m_camera, m_renderTarget);

                const ImVec2 imagePosition = ImGui::GetCursorScreenPos();

                const ImVec2 imageEnd =
                {
                    imagePosition.x + imageWidth,
                    imagePosition.y + imageHeight
                };

                ImGui::GetWindowDrawList()->AddImage(
                    (ImTextureID)renderSystem.getTextureId(
                        m_renderTarget.colorTexture()),
                    imagePosition,
                    imageEnd,
                    {0, 1},
                    {1, 0});

                const bool sceneHovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(imagePosition, imageEnd);

                if (sceneHovered)
                {
                    const ImGuiIO& io = ImGui::GetIO();

                    camera::DragInput input;

                    input.mouseDelta =
                    {
                        -io.MouseDelta.x,
                        -io.MouseDelta.y
                    };

                    input.scrollDelta =
                    {
                        -io.MouseWheelH,
                        -io.MouseWheel
                    };

                    input.leftMouse = ImGui::IsMouseDown(ImGuiMouseButton_Left);
                    input.rightMouse = ImGui::IsMouseDown(ImGuiMouseButton_Right);
                    input.middleMouse = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
                    input.alt = io.KeyAlt;

                    camera::updateDrag(m_camera, input);
                }

                GameObject* selected = context.selectedGameObject();

                if (selected != nullptr)
                {
                    ImGuizmo::SetOrthographic(m_camera.getCameraType() == Camera::CameraType::Orthographic);

                    ImGuizmo::SetDrawlist();

                    ImGuizmo::SetRect(imagePosition.x, imagePosition.y, imageWidth, imageHeight);

                    const mat4 view =m_camera.getViewMatrix();

                    const mat4 projection =m_camera.getProjectionMatrix(
                            {
                                static_cast<int>(imageWidth),
                                static_cast<int>(imageHeight)
                            });

                    Transform& transform = selected->transform();

                    mat4 model = transform.getWorldMatrix();

                    const bool manipulated = ImGuizmo::Manipulate(view.data(), projection.data(), ImGuizmo::TRANSLATE, ImGuizmo::WORLD, model.data());

                    const bool usingGizmo = ImGuizmo::IsUsing();

                    if (usingGizmo && !m_wasUsingGizmo)
                    {
                        m_gizmoStartTransform = captureTransform(transform);
                    }

                    if (manipulated)
                    {
                        mat4 localMatrix = model;

                        if (Transform* parent = transform.getParent())
                        {
                            localMatrix = math::inverse(parent->getWorldMatrix()) * model;
                        }

                        float translation[3];
                        float rotation[3];
                        float scale[3];

                        ImGuizmo::DecomposeMatrixToComponents(localMatrix.data(), translation, rotation, scale);

                        transform.position =
                        {
                            translation[0],
                            translation[1],
                            translation[2]
                        };

                        transform.rotation =
                        {
                            rotation[0],
                            rotation[1],
                            rotation[2]
                        };

                        transform.scale =
                        {
                            scale[0],
                            scale[1],
                            scale[2]
                        };
                    }

                    if (!usingGizmo && m_wasUsingGizmo)
                    {
                        const TransformState finalTransform = captureTransform(transform);
                        context.commands().pushExecuted<TransformCommand>(context.sceneSystem(), selected->uuid(), m_gizmoStartTransform, finalTransform);
                    }

                    m_wasUsingGizmo = usingGizmo;
                }
            }
        }

        ImGui::End();
    }

}
