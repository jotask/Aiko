#pragma once

#include "ui_types.h"
#include "models/texture_region.h"

#include <aiko_types.h>
#include <assets/asset_id.h>

namespace aiko
{

    class UIDrawList
    {
    public:

        void clear();

        void addRect(const UIRect& rect, Color color);

        void addImage(const UIRect& rect, AssetId textureId, Color tint = WHITE);
        void addImage(const UIRect& rect, AssetId textureId, const TextureRegion& region, Color tint = WHITE);

        void pushClipRect(const UIRect& rect);
        void popClipRect();

        const vector<UIDrawCommand>& commands() const
        {
            return m_commands;
        }

        const vector<UIVertex>& vertices() const
        {
            return m_vertices;
        }

        const vector<uint16_t>& indices() const
        {
            return m_indices;
        }

        bool empty() const
        {
            return m_vertices.empty();
        }

    private:

        vector<UIDrawCommand> m_commands;
        vector<UIVertex> m_vertices;
        vector<uint16_t> m_indices;

        vector<UIRect> m_clipStack;

        void addQuad(const UIRect& rect, Color color, AssetId textureId, const TextureRegion& region);

    };

}
