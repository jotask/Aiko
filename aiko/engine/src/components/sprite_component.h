#pragma once

#include <aiko_types.h>
#include <models/mesh.h>

#include "models/component.h"

namespace aiko
{

    class SpriteComponent : public Component
    {
    public:

        SpriteComponent(size_t, size_t);
        SpriteComponent(string files);
        virtual ~SpriteComponent() = default;

        virtual void init() override;

        void load(string);
        void create(size_t width, size_t height);

        // TODO TMP for now
        void setPixel(size_t x, size_t y, Color c);
        void setPixels(std::vector<Color> pixels);
        void refresh();

        Mesh& getMesh();
        Material& getMaterial();

    private:
        Mesh            m_mesh;
        Material        m_material;

        std::vector<Color> pixels;
        bool is_dirty = false;

    };

}