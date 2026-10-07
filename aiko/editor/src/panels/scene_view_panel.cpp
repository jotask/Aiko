#include "scene_view_panel.h"

#include "camera/camera_controller.h"
#include "commands/transform/transform_command.h"
#include "core/editor_context.h"
#include "models/game_object.h"
#include "systems/render_system.h"

#include "scene/scene_picking.h"

#include <math/math.h>
#include <math/math_bounds.h>

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
            const bool translateActive = m_gizmoOperation == ImGuizmo::TRANSLATE;
            const bool rotateActive = m_gizmoOperation == ImGuizmo::ROTATE;
            const bool scaleActive = m_gizmoOperation == ImGuizmo::SCALE;

            ImGui::BeginDisabled(translateActive);
            if (ImGui::Button("W"))
            {
                m_gizmoOperation = ImGuizmo::TRANSLATE;
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(rotateActive);
            if (ImGui::Button("E"))
            {
                m_gizmoOperation = ImGuizmo::ROTATE;
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(scaleActive);
            if (ImGui::Button("R"))
            {
                m_gizmoOperation = ImGuizmo::SCALE;
            }
            ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::BeginDisabled(scaleActive);

            const char* modeLabel = m_gizmoMode == ImGuizmo::WORLD ? "World" : "Local";

            if (ImGui::Button(modeLabel))
            {
                m_gizmoMode = m_gizmoMode == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
            }

            ImGui::EndDisabled();

            RenderSystem& renderSystem = context.renderSystem();

            const ImVec2 availableSpace = ImGui::GetContentRegionAvail();

            const float imageWidth = availableSpace.x;
            const float imageHeight = availableSpace.y;

            if (imageWidth > 0.0f && imageHeight > 0.0f)
            {
                const u32 targetWidth = static_cast<u32>(imageWidth);
                const u32 targetHeight = static_cast<u32>(imageHeight);

                const ivec2 currentSize = m_renderTarget.size();

                if (currentSize.x != static_cast<int>(targetWidth) ||currentSize.y != static_cast<int>(targetHeight))
                {
                    m_renderTarget.resize(targetWidth, targetHeight);
                }

                const ImVec2 imagePosition = ImGui::GetCursorScreenPos();

                const ImVec2 imageEnd =
                {
                    imagePosition.x + imageWidth,
                    imagePosition.y + imageHeight
                };

                const bool sceneHovered = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(imagePosition, imageEnd);

                GameObject* selected = context.selectedGameObject();

                //
                // Editor camera
                //

                if (sceneHovered && !ImGuizmo::IsUsing())
                {
                    const ImGuiIO& io = ImGui::GetIO();

                    const bool rightMouse = ImGui::IsMouseDown(ImGuiMouseButton_Right);

                    if (rightMouse)
                    {
                        //
                        // Unity-style fly camera
                        //

                        const float targetDistance = math::length(m_camera.target - m_camera.position);

                        vec3 forward = math::normalize(m_camera.target - m_camera.position);
                        vec3 right = math::normalize(math::cross(forward,m_camera.getUp()));

                        const vec2 mouseDelta =
                        {
                            -io.MouseDelta.x,
                            -io.MouseDelta.y
                        };

                        constexpr float lookSensitivity = 0.15f;
                        const float yaw = mouseDelta.x * lookSensitivity;
                        const float pitch = mouseDelta.y * lookSensitivity;

                        forward = math::rotate(forward, pitch, right);
                        forward = math::rotate(forward, yaw, m_camera.getUp());
                        forward = math::normalize(forward);
                        right = math::normalize(math::cross(forward, m_camera.getUp()));

                        vec3 movement = {};

                        if (ImGui::IsKeyDown(ImGuiKey_W))
                        {
                            movement += forward;
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_S))
                        {
                            movement -= forward;
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_A))
                        {
                            movement -= right;
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_D))
                        {
                            movement += right;
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_Q))
                        {
                            movement -= m_camera.getUp();
                        }

                        if (ImGui::IsKeyDown(ImGuiKey_E))
                        {
                            movement += m_camera.getUp();
                        }

                        float speed = m_flySpeed;

                        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
                        {
                            speed *= 2.0f;
                        }

                        if (math::length(movement) > 0.0f)
                        {
                            movement = math::normalize(movement);
                        }

                        const vec3 displacement = movement * speed * io.DeltaTime;

                        m_camera.position += displacement;

                        //
                        // Preserve the current orbit/focus distance.
                        //

                        m_camera.target = m_camera.position + forward * targetDistance;
                    }
                    else
                    {
                        //
                        // Existing orbit / pan / zoom controls
                        //

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
                        input.rightMouse = false;
                        input.middleMouse = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
                        input.alt = io.KeyAlt;

                        camera::updateDrag(m_camera, input);
                    }
                }

                //
                // Scene shortcuts
                //

                if (sceneHovered && !ImGuizmo::IsUsing() && !ImGui::IsMouseDown(ImGuiMouseButton_Right))
                {
                    if (ImGui::IsKeyPressed(ImGuiKey_W))
                    {
                        m_gizmoOperation = ImGuizmo::TRANSLATE;
                    }

                    if (ImGui::IsKeyPressed(ImGuiKey_E))
                    {
                        m_gizmoOperation = ImGuizmo::ROTATE;
                    }

                    if (ImGui::IsKeyPressed(ImGuiKey_R))
                    {
                        m_gizmoOperation = ImGuizmo::SCALE;
                    }

                    if (ImGui::IsKeyPressed(ImGuiKey_X))
                    {
                        m_gizmoMode = m_gizmoMode == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
                    }

                    if (ImGui::IsKeyPressed(ImGuiKey_F) && selected != nullptr)
                    {
                        const mat4 world =
                            selected->transform().getWorldMatrix();

                        const vec3 worldPosition =
                        {
                            world(0, 3),
                            world(1, 3),
                            world(2, 3)
                        };

                        const vec3 offset = m_camera.position - m_camera.target;

                        m_camera.target = worldPosition;

                        m_camera.position = worldPosition + offset;
                    }
                }

                //
                // Render Scene View using the updated editor camera
                //

                renderSystem.renderToTarget(m_camera, m_renderTarget);

                ImGui::Image(
                    (ImTextureID)renderSystem.getTextureId(
                        m_renderTarget.colorTexture()),
                    {
                        imageWidth,
                        imageHeight
                    },
                    {0, 1},
                    {1, 0});

                //
                // ImGuizmo viewport
                //

                ImGuizmo::SetOrthographic(m_camera.getCameraType() == Camera::CameraType::Orthographic);

                ImGuizmo::SetDrawlist();

                ImGuizmo::SetRect(imagePosition.x, imagePosition.y, imageWidth, imageHeight);

                const mat4 view = m_camera.getViewMatrix();

                const mat4 projection =
                    m_camera.getProjectionMatrix(
                        {
                            static_cast<int>(imageWidth),
                            static_cast<int>(imageHeight)
                        });

                //
                // Grid
                //

                if (context.viewSettings().showGrid)
                {
                    const mat4 gridMatrix(1.0f);
                    ImGuizmo::DrawGrid(view.data(), projection.data(), gridMatrix.data(), 100.0f);
                }

                //
                // Selected object transform gizmo
                //

                if (selected != nullptr)
                {
                    Transform& transform = selected->transform();

                    mat4 model = m_wasUsingGizmo ? m_gizmoMatrix : transform.getWorldMatrix();

                    const ImGuizmo::MODE gizmoMode = m_gizmoOperation == ImGuizmo::SCALE ? ImGuizmo::LOCAL : m_gizmoMode;

                    const bool manipulated = ImGuizmo::Manipulate(view.data(), projection.data(), m_gizmoOperation, gizmoMode, model.data());

                    const bool usingGizmo = ImGuizmo::IsUsing();

                    if (usingGizmo && !m_wasUsingGizmo)
                    {
                        m_gizmoStartTransform = captureTransform(transform);
                    }

                    if (usingGizmo)
                    {
                        m_gizmoMatrix = model;
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

                        switch (m_gizmoOperation)
                        {
                            case ImGuizmo::TRANSLATE:
                            {
                                transform.position =
                                {
                                    translation[0],
                                    translation[1],
                                    translation[2]
                                };
                            }
                            break;

                            case ImGuizmo::ROTATE:
                            {
                                transform.rotation =
                                {
                                    rotation[0],
                                    rotation[1],
                                    rotation[2]
                                };
                            }
                            break;

                            case ImGuizmo::SCALE:
                            {
                                transform.scale =
                                {
                                    scale[0],
                                    scale[1],
                                    scale[2]
                                };
                            }
                            break;

                            default:
                                break;
                        }
                    }

                    if (!usingGizmo && m_wasUsingGizmo)
                    {
                        const TransformState finalTransform = captureTransform(transform);

                        context.commands()
                            .pushExecuted<TransformCommand>(
                                context.sceneSystem(),
                                selected->uuid(),
                                m_gizmoStartTransform,
                                finalTransform);
                    }

                    m_wasUsingGizmo = usingGizmo;
                }
                else
                {
                    m_wasUsingGizmo = false;
                }

                //
                // Scene picking
                //

                if (sceneHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver())
                {
                    const ImVec2 mousePosition = ImGui::GetMousePos();

                    const vec2 viewportPosition =
                    {
                        (mousePosition.x - imagePosition.x) / imageWidth,
                        (mousePosition.y - imagePosition.y) / imageHeight
                    };

                    const Ray ray = math::unprojectRay(viewportPosition, view, projection);

                    const ScenePickResult pick = pickScene(context, ray);

                    if (pick)
                    {
                        context.select(pick.object);
                    }
                    else
                    {
                        context.clearSelection();
                    }
                }

            }
        }

        ImGui::End();
    }

}
