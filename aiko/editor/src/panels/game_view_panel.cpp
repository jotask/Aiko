#include "game_view_panel.h"

#include "systems/render_system.h"
#include "core/editor_context.h"

#include <aiko_includes.h>

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        GameViewPanel::GameViewPanel()
            : EditorPanel("Game")
        {
        }

        void GameViewPanel::render(EditorContext& context)
        {
            RenderSystem& renderSystem = context.renderSystem();

            if (ImGui::Begin("Game"))
            {
                if (ImGui::BeginChild("GameRender"))
                {
                    const ImVec2 availableSpace = ImGui::GetContentRegionAvail();

                    if (availableSpace.x > 0.0f && availableSpace.y > 0.0f)
                    {
                        const FrameBuffer& target = renderSystem.getTargetTexture();
                        const Texture& texture = target.getColorTexture();

                        const ImVec2 textureSize(texture.getInfo().width, texture.getInfo().height);

                        if (textureSize.x > 0.0f && textureSize.y > 0.0f)
                        {
                            const float aspectRatio = textureSize.x / textureSize.y;

                            float imageWidth;
                            float imageHeight;

                            if (availableSpace.x / availableSpace.y > aspectRatio)
                            {
                                imageHeight = availableSpace.y;
                                imageWidth = imageHeight * aspectRatio;
                            }
                            else
                            {
                                imageWidth = availableSpace.x;
                                imageHeight = imageWidth / aspectRatio;
                            }

                            const ImVec2 pos = ImGui::GetCursorScreenPos();

                            ImGui::GetWindowDrawList()->AddImage(
                                (ImTextureID)renderSystem.getTargetTextureId(),
                                ImVec2(pos.x, pos.y),
                                ImVec2(pos.x + imageWidth, pos.y + imageHeight),
                                ImVec2(0, 1),
                                ImVec2(1, 0)
                            );
                        }
                    }
                }

                ImGui::EndChild();
            }

            ImGui::End();
        }

    }
}
