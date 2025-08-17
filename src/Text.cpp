#include "Text.h"

Text::Text(std::string text)
    : m_text(text)
{
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

//! \brief positions the text such that it is centered around \center
//! \param center
void Text::centerAround(const utils::Vector2f &center)
{
    utils::Vector2f new_pos = center;

    auto highest_char =
        *std::max_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          { return m_font->m_characters.at(c1).size.y < m_font->m_characters.at(c2).size.y; });
    auto max_height = m_font->m_characters.at(highest_char).bearing.y - m_font->m_characters.at(highest_char).size.y / 2.f;
    new_pos.y -= max_height;

    auto width = getTextWidth();
    new_pos.x -= width / 2.f;

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

    //     // ! find dimensions of the text
    auto text_size_x =
        std::accumulate(m_text.begin(), m_text.end(), 0, [this](int x, auto character)
                        { return m_font->m_characters.at(character).size.x + x; });
    auto largest_char =
        *std::max_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.size.y < car2.bearing.y; });
    auto lowest_char =
        *std::max_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.size.y - car1.bearing.y < car2.size.y - car2.bearing.y; });

    auto largest_c = m_font->m_characters.at(largest_char);
    auto lowest_c = m_font->m_characters.at(lowest_char);
    utils::Vector2f text_size = {text_size_x, largest_c.bearing.y + lowest_c.size.y - lowest_c.bearing.y};

    Rect<float> bounding_box;
    text_size.x *= getScale().x;
    text_size.y *= getScale().y;
    bounding_box.width = 0;
    bounding_box.pos_y = getPosition().y - text_size.y / 2.f;
    bounding_box.height = text_size.y;

    bounding_box.pos_y += 2. * (lowest_c.size.y - lowest_c.bearing.y) * getScale().y;

    for (auto c : m_text)
    {
        auto character = m_font->m_characters.at(c);
        bounding_box.width += (character.advance >> 6) * getScale().x;
    }
    bounding_box.pos_x = getPosition().x;

    return bounding_box;
}

#include "Renderer.h"

void MultiLineText::drawInto(Renderer &canvas)
{

    utils::Vector2f word_pos = m_page_position + m_page_padding;
    auto drawWordAndMoveCursor = [&, this](Text &t_word)
    {
        auto b_box = t_word.getBoundingBox();
        if (word_pos.x + b_box.width > rightTextBorder()) //! line overflow -> newline
        {
            word_pos.x = leftTextBorder();
            word_pos.y += m_line_size.y + m_line_spacing;
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
    t_word.setScale(1, -1); //! fuck the flipping, fuck OpenGL coordinates, and fuck me

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
}