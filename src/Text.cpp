#include "Text.h"
#include "Renderer.h"

Text::Text(std::string text)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    m_text = conv.from_bytes(text);
}

void Text::setFont(Font *new_font)
{
    m_font = new_font;
}
Font *Text::getFont() const
{
    return m_font;
}

void Text::setText(const std::string &new_text)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    m_text = conv.from_bytes(new_text);
}

std::string Text::getText() const
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(m_text);
}

void Text::setColor(ColorByte new_color)
{
    m_color = new_color;
}

const ColorByte &Text::getColor() const
{
    return m_color;
}

//! \brief positions the text such that it is centered around \center
//! \param center
void Text::centerAround(const utils::Vector2f &center)
{
    utils::Vector2f new_pos = center;

    auto bb = getBoundingBox();

    new_pos.y -= bb.height / 2.f;
    new_pos.x -= bb.width / 2.f;

    setPosition(new_pos);
}

//! \returns calculates the width of the text;
//! TODO: this should probably be kept as a parameter in the class
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
        width += (character.advance >> 6) * getScale().x;
    }
    return width;
}

//! \todo I should probably cache these results since they are unlikely to change
Rect<float> Text::getBoundingBox() const
{
    if (m_text.empty())
    {
        return {getPosition().x, getPosition().y, 0, 0};
    }

    //     // ! find dimensions of the text
    auto text_size_x =
        std::accumulate(m_text.begin(), m_text.end(), 0, [this](int x, int character)
                        { return m_font->m_characters.at(character).size.x + x; });
    auto largest_char =
        *std::max_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.bearing.y < car2.bearing.y; });
    auto lowest_char =
        *std::max_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.size.y - car1.bearing.y < car2.size.y - car2.bearing.y; });

    auto largest_c = m_font->m_characters.at(largest_char);
    auto lowest_c = m_font->m_characters.at(lowest_char);
    utils::Vector2f text_size = {text_size_x, largest_c.bearing.y + (lowest_c.size.y - lowest_c.bearing.y)};

    Rect<float> bounding_box;
    text_size.x *= getScale().x;
    text_size.y *= getScale().y;
    bounding_box.pos_y = getPosition().y - text_size.y / 2.f;
    bounding_box.height = text_size.y;

    bounding_box.pos_y += 1. * (lowest_c.size.y - lowest_c.bearing.y) * getScale().y;

    bounding_box.width = 0;
    for (auto c : m_text)
    {
        auto character = m_font->m_characters.at(c);
        bounding_box.width += (character.advance >> 6) * getScale().x;
    }
    bounding_box.pos_x = getPosition().x;

    return bounding_box;
}


float largestCharacterHeight(Font &font)
{
    auto max_el_it = std::max_element(font.m_characters.begin(), font.m_characters.end(), [](auto &character1, auto &character2)
                                      {
        Character& glyph1 = character1.second;
        Character& glyph2 = character2.second;
        return glyph1.size.y < glyph2.size.y; });
    return (float)(max_el_it->second.size.y);
}

void MultiLineText::drawInto(Renderer &canvas)
{
    m_line_size = m_text_scale * largestCharacterHeight(*p_font);

    utils::Vector2f word_pos = m_page_position + m_page_padding + utils::Vector2f{0.f, m_line_size};
    auto drawWordAndMoveCursor = [&, this](Text &t_word)
    {
        auto b_box = t_word.getBoundingBox();
        if (word_pos.x + b_box.width > rightTextBorder()) //! line overflow -> newline
        {
            word_pos.x = leftTextBorder();
            word_pos.y += m_line_size + m_line_spacing;
        }

        t_word.setPosition(word_pos);
        canvas.drawText(t_word);
        word_pos.x += b_box.width + m_word_spacing;
    };

    //! iterate through words
    std::size_t start_pos = 0;
    std::size_t next_pos = m_text.find_first_of(' ');
    Text t_word = {m_text.substr(start_pos, next_pos - start_pos + 1)};
    t_word.setFont(p_font);
    t_word.setScale(m_text_scale, -m_text_scale); //! fuck the flipping, fuck OpenGL coordinates, and fuck me

    while (next_pos != std::string::npos)
    {
        t_word.setText(m_text.substr(start_pos, next_pos - start_pos + 1));

        drawWordAndMoveCursor(t_word);

        start_pos = next_pos + 1;
        next_pos = m_text.find_first_of(' ', start_pos + 1);
    }

    //! draw the last word
    t_word.setText(m_text.substr(start_pos));
    drawWordAndMoveCursor(t_word);

    m_page_height = m_page_padding.y + word_pos.y - m_page_position.y;
}