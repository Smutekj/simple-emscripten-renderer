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