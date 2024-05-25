#include "nes_emulator_component.h"

#include  <algorithm>

#include "models/game_object.h"
#include "systems/render_system.h"

namespace nes
{

    NesComponent::NesComponent()
        : PboTextureComponent("NesComponent", 256, 240, true)
    {

    }

    void NesComponent::init()
    {
        auto_render = false;
        PboTextureComponent::init();
    }

    void NesComponent::update()
    {
        auto_render = true;
        PboTextureComponent::update();
        auto_render = false;
    }

    void NesComponent::render()
    {
        PboTextureComponent::render();
    }

    aiko::texture::Texture NesComponent::getTexture()
    {
        return m_texture.texture;
    }

}
