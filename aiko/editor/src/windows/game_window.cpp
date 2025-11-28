#include "game_window.h"

#include "aiko_editor.h"
#include "aiko.h"
#include "systems/render_system.h"

#include <aiko_includes.h>

#include "window.h"

#include <imgui.h>

namespace aiko
{
    namespace editor
    {

        GameWindow::GameWindow(AikoEditor* editor)
            : Window(editor, "GameWindow")
        {

        }

        void GameWindow::render()
        {

            static auto* renderSystem = m_editor->getAiko()->getSystem<aiko::RenderSystem>();
            if(ImGui::Begin("Game"))
            {

                aiko::texture::RenderTexture2D* aikoTarget = renderSystem->getTargetTexture();

                aiko::texture::Texture texture = aikoTarget->texture;
                // Using a Child allow to fill all the space of the window.
                // It also allows customization
                ImGui::BeginChild("GameRender");
                // Get the size of the child (i.e. the whole draw size of the windows).

                // Get the dimensions of the texture
                ImVec2 textureSize = ImVec2(texture.width, texture.height);

                // Calculate aspect ratio of the image
                float aspectRatio = textureSize.x / textureSize.y;

                // Get available space in the window
                ImVec2 availableSpace = ImGui::GetContentRegionAvail();

                // Calculate the maximum size of the image while maintaining the aspect ratio
                float maxWidth = availableSpace.x;
                float maxHeight = availableSpace.y;
                float imageWidth, imageHeight;

                if (maxWidth / maxHeight > aspectRatio)
                {
                    // Height is the limiting factor
                    imageHeight = maxHeight;
                    imageWidth = maxHeight * aspectRatio;
                }
                else
                {
                    // Width is the limiting factor
                    imageWidth = maxWidth;
                    imageHeight = maxWidth / aspectRatio;
                }

                // Ensure that the image does not exceed the available space
                imageWidth = std::min(imageWidth, maxWidth);
                imageHeight = std::min(imageHeight, maxHeight);

                // we get the screen position of the window
                ImVec2 pos = ImGui::GetCursorScreenPos();

                // and here we can add our created texture as image to ImGui
                // unfortunately we need to use the cast to void* or I didn't find another way tbh
                ImGui::GetWindowDrawList()->AddImage(
                    (ImTextureID)texture.id,
                    ImVec2(pos.x, pos.y),
                    ImVec2(pos.x + imageWidth, pos.y + imageHeight),
                    ImVec2(0, 1),
                    ImVec2(1, 0)
                );

                ImGui::EndChild();
            }
            ImGui::End();
        }

    }
}
