#pragma once

#include "ui_types.h"

#include <aiko_types.h>

namespace aiko
{

    class UIDrawList
    {
    public:

        void clear();

        void addRect(const UIRect& rect, Color color);

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

        vector<UIVertex> m_vertices;
        vector<uint16_t> m_indices;
    };

}
