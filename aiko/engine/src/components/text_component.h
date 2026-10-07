#pragma once

#include <string>

#include <types/color.h>

#include "models/component.h"
#include "models/font.h"

namespace aiko
{

    class TextComponent : public Component
    {
    public:

        TextComponent();
        virtual ~TextComponent() = default;

        void setText(string text)
        {
            m_text = std::move(text);
        }

        const string& getText() const
        {
            return m_text;
        }

        void setFont(const Font& font)
        {
            m_font = font;
        }

        const Font& getFont() const
        {
            return m_font;
        }

        void setFontSize(float size)
        {
            AIKO_ASSERT(size > 0.0f, "Text font size must be greater than zero");
            m_fontSize = size;
        }

        float getFontSize() const
        {
            return m_fontSize;
        }

        void setColor(Color color)
        {
            m_color = color;
        }

        Color getColor() const
        {
            return m_color;
        }

    private:

        string m_text;
        Font m_font;

        float m_fontSize = 16.0f;
        Color m_color = WHITE;
    };

}
