#include "ui_draw_list.h"

#include <algorithm>
#include <limits>

namespace aiko
{

    void UIDrawList::clear()
    {
        m_vertices.clear();
        m_indices.clear();
        m_commands.clear();
        m_clipStack.clear();
    }

    void UIDrawList::addRect(const UIRect& rect, Color color, float cornerRadius, float borderThickness, Color borderColor)
    {
        addQuad(rect, color, InvalidAssetId, TextureRegion::full(), cornerRadius, borderThickness, borderColor);
    }

    void UIDrawList::addImage(const UIRect& rect, AssetId textureId, Color tint, float cornerRadius, float borderThickness, Color borderColor)
    {
        AIKO_ASSERT(textureId != InvalidAssetId, "UI image requires a valid texture asset");
        addImage(rect, textureId, TextureRegion::full(), tint, cornerRadius, borderThickness, borderColor);
    }

    void UIDrawList::addImage(const UIRect& rect, AssetId textureId, const TextureRegion& region, Color tint, float cornerRadius, float borderThickness, Color borderColor)
    {
        AIKO_ASSERT(textureId != InvalidAssetId, "UI image requires a valid texture asset");
        addQuad(rect, tint, textureId, region, cornerRadius, borderThickness, borderColor);
    }

    void UIDrawList::pushClipRect(const UIRect& rect)
    {
        AIKO_ASSERT(rect.size.x >= 0.0f, "UI clip width must not be negative");
        AIKO_ASSERT(rect.size.y >= 0.0f, "UI clip height must not be negative");

        UIRect effective = rect;

        if (m_clipStack.empty() == false)
        {
            const UIRect& parent = m_clipStack.back();

            const float left = std::max(parent.position.x, rect.position.x);
            const float top = std::max(parent.position.y, rect.position.y);
            const float right = std::min(parent.position.x + parent.size.x, rect.position.x + rect.size.x);
            const float bottom = std::min(parent.position.y + parent.size.y, rect.position.y + rect.size.y);

            effective.position =
            {
                left,
                top
            };

            effective.size =
            {
                std::max(0.0f, right - left),
                std::max(0.0f, bottom - top)
            };
        }

        m_clipStack.push_back(effective);
    }

    void UIDrawList::popClipRect()
    {
        AIKO_ASSERT(m_clipStack.empty() == false, "UI clip stack is empty");
        m_clipStack.pop_back();
    }

    void UIDrawList::addQuad(const UIRect& rect, Color color, AssetId textureId, const TextureRegion& region, float cornerRadius, float borderThickness, Color borderColor)
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

        UIDrawCommand command =
        {
            .indexOffset = indexOffset,
            .indexCount = 6,
            .textureId = textureId,
            .rect = rect,
            .cornerRadius = cornerRadius,
            .borderThickness = borderThickness,
            .borderColor = borderColor
        };

        if (m_clipStack.empty() == false)
        {
            command.clipRect = m_clipStack.back();
        }

        m_commands.push_back(command);
    }
}
