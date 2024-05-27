#include "components_render.h"

#include <assert.h>

#include <imgui.h>

namespace editor
{
    namespace component
    {

        const void drawComponent(aiko::Component* compt)
        {
            if (aiko::Transform* trans = dynamic_cast<aiko::Transform*>(compt))
            {
                drawTransform(trans);
            }
            else if (aiko::TextureComponent* text = dynamic_cast<aiko::TextureComponent*>(compt))
            {
                drawTexture(text);
            }
            else if (aiko::PboTextureComponent* pbo = dynamic_cast<aiko::PboTextureComponent*>(compt))
            {
                drawPboTexture(pbo);
            }
            else if (aiko::MeshComponent* mesh = dynamic_cast<aiko::MeshComponent*>(compt))
            {
                drawMesh(mesh);
            }
            else
            {
                assert(false, "ERROR :: Component is not supported by the editor");
            }
        }

        const void drawTransform(aiko::Transform* t)
        {
            ImGui::DragFloat3("Position", t->position);
            ImGui::DragFloat3("Rotation", t->rotation);
            ImGui::DragFloat3("Scale", t->scale);

        }

        const void drawTexture(aiko::TextureComponent* text)
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

        const void drawPboTexture(aiko::PboTextureComponent* pbo)
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

        const void drawMesh(aiko::MeshComponent* mesh)
        {

        }

    }
}