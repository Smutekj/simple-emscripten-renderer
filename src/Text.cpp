#include "Text.h"

Text::Text(std::string text)
    : m_text(text)
{
}

void Text::setFont(Font *new_font)
{
    m_font = new_font;
}
Font *Text::getFont()
{
    return m_font;
}

void Text::setText(const std::string &new_text)
{
    m_text = new_text;
}

const std::string &Text::getText() const
{
    return m_text;
}

void Text::setColor(ColorByte new_color)
{
    m_color = new_color;
}

const ColorByte &Text::getColor() const
{
    return m_color;
}

void Text::centerAround(const utils::Vector2f& center)
{
    auto width = getTextWidth();
    auto curr_pos = getPosition();
    utils::Vector2f new_pos = center;
    new_pos.x -= width/2.f;
    setPosition(new_pos);
}

float Text::getTextWidth() const
{
    if (!m_font)
    {
        return 0.f;
    }

    float width = 0.f;
    for (std::size_t glyph_ind = 0; glyph_ind < m_text.size(); ++glyph_ind)
    {
        auto character = m_font->m_characters.at(m_text.at(glyph_ind));

        float dy = character.size.y - character.bearing.y;
        float height = character.size.y * getScale().y;

        width += (character.advance >> 6) * getScale().x;
    }
    return width;
}
