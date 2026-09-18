#include "ui_draw_list.h"

#include <limits>

namespace aiko
{

    void UIDrawList::clear()
    {
        m_vertices.clear();
        m_indices.clear();
        m_commands.clear();
    }

    void UIDrawList::addRect(const UIRect& rect, Color color)
    {
        addQuad(rect, color, InvalidAssetId, TextureRegion::full());
    }

    void UIDrawList::addImage(const UIRect& rect, AssetId textureId, Color tint)
    {
        AIKO_ASSERT(textureId != InvalidAssetId, "UI image requires a valid texture asset");
        addImage(rect, textureId, TextureRegion::full(), tint);
    }

    void UIDrawList::addImage(const UIRect& rect, AssetId textureId, const TextureRegion& region, Color tint)
    {
        AIKO_ASSERT(textureId != InvalidAssetId, "UI image requires a valid texture asset");
        addQuad(rect, tint, textureId, region);
    }

    void UIDrawList::addQuad(const UIRect& rect, Color color, AssetId textureId, const TextureRegion& region)
    {
        AIKO_ASSERT(m_vertices.size() <= std::numeric_limits<uint16_t>::max() - 4, "UIDrawList exceeded the 16-bit vertex limit");

        const uint32_t indexOffset = static_cast<uint32_t>(m_indices.size());

        const uint16_t base = static_cast<uint16_t>(m_vertices.size());

        const float left = rect.position.x;
        const float top = rect.position.y;

        const float right = rect.position.x + rect.size.x;

        const float bottom = rect.position.y + rect.size.y;

        m_vertices.push_back(
        {
            .position = {left, top},
            .uv = {region.min.x, region.min.y},
            .color = color
        });

        m_vertices.push_back(
        {
            .position = {right, top},
            .uv = {region.max.x, region.min.y},
            .color = color
        });

        m_vertices.push_back(
        {
            .position = {left, bottom},
            .uv = {region.min.x, region.max.y},
            .color = color
        });

        m_vertices.push_back(
        {
            .position = {right, bottom},
            .uv = {region.max.x, region.max.y},
            .color = color
        });

        m_indices.push_back(base + 0);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 1);

        m_indices.push_back(base + 1);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 3);

        m_commands.push_back(
        {
            .indexOffset = indexOffset,
            .indexCount = 6,
            .textureId = textureId
        });
    }
}
