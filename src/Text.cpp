#include "Text.h"
#include "Renderer.h"

#include <codecvt>

Text::Text(std::string text)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    m_text = conv.from_bytes(text);
}
const std::wstring &Text::getTextW() const
{
    return m_text;
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

void Text::centerAroundX(float center_x)
{
    utils::Vector2f new_pos = {center_x, getPosition().y};

    auto bb = getBoundingBox();
    new_pos.x -= bb.width / 2.f;

    setPosition(new_pos);
}
void Text::centerAroundY(float center_y)
{
    utils::Vector2f new_pos = {getPosition().x, center_y};

    auto bb = getBoundingBox();
    float depth = getDepthUnderLine();
    new_pos.y -= bb.height / 2.f;
    new_pos.y += depth;

    setPosition(new_pos);
}
void Text::centerAround(const utils::Vector2f &center)
{
    utils::Vector2f new_pos = center;

    auto bb = getBoundingBox();
    float depth = getDepthUnderLine();

    new_pos.y -= bb.height / 2.f;
    new_pos.y += depth;
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

float Text::getDepthUnderLine() const
{
    if (m_text.empty())
    {
        return 0.f;
    }

    auto lowest_pos_char =
        *std::min_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.bb.pos_y < car2.bb.pos_y; });

    auto lowest_c = m_font->m_characters.at(lowest_pos_char);
    return -(lowest_c.bb.pos_y) * getScale().y;
}

//! \todo I should probably cache these results since they are unlikely to change
Rect<float> Text::getBoundingBox() const
{
    if (m_text.empty())
    {
        return {getPosition().x, getPosition().y, 0, 0};
    }

    //     // ! find dimensions of the text
    auto largest_pos_char =
        *std::max_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.bb.pos_y + car1.bb.height < car2.bb.pos_y + car2.bb.height; });
    auto lowest_pos_char =
        *std::min_element(m_text.begin(), m_text.end(), [this](auto c1, auto c2)
                          {
        auto car1 = m_font->m_characters.at(c1);
        auto car2 = m_font->m_characters.at(c2);
        return car1.bb.pos_y < car2.bb.pos_y; });

    auto largest_c = m_font->m_characters.at(largest_pos_char);
    auto lowest_c = m_font->m_characters.at(lowest_pos_char);
    float text_height = largest_c.bb.height - lowest_c.bb.pos_y;

    Rect<float> bounding_box;
    text_height *= getScale().y;
    bounding_box.pos_y = getPosition().y;
    bounding_box.height = text_height;

    bounding_box.pos_y -= (-lowest_c.bb.pos_y) * getScale().y;

    bounding_box.width = 0;
    for (auto c : m_text)
    {
        auto character = m_font->m_characters.at(c);
        bounding_box.width += (character.advance >> 6) * getScale().x;
    }
    bounding_box.pos_x = getPosition().x;
    // std::cout << "TEXT: " << getText() << " BB:\n" << bounding_box.height<< " \nbelow: " << (lowest_c.size.y - lowest_c.bearing.y) * getScale().y << std::endl;

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

void MultiLineText::drawInto2(Renderer &canvas)
{
    m_line_size = m_text_scale * p_font->getLineHeight();

    utils::Vector2f word_pos = m_page_position + Vec2{m_page_padding.x, -m_page_padding.y} - utils::Vector2f{0.f, m_line_size};
    auto drawWordAndMoveCursor = [&, this](Text &t_word)
    {
        auto b_box = t_word.getBoundingBox();
        if (word_pos.x + b_box.width > rightTextBorder()) //! line overflow -> newline
        {
            word_pos.x = leftTextBorder();
            word_pos.y -= m_line_size + m_line_spacing;
        }

        t_word.setPosition(word_pos);
        canvas.drawText(t_word);
        word_pos.x += b_box.width + m_word_spacing;
    };

    //! iterate through words
    std::size_t start_pos = 0;
    std::size_t next_pos = m_text.find_first_of(' ');
    Text t_word = Text{m_text.substr(start_pos, next_pos - start_pos + 1)};
    t_word.setFont(p_font);
    t_word.setScale(m_text_scale, m_text_scale); //! fuck the flipping, fuck OpenGL coordinates, and fuck me

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

    m_page_height = m_page_padding.y + m_line_size + m_line_spacing - (word_pos.y - m_page_position.y);
}

void MultiLineText::drawInto(Renderer &canvas)
{
    m_line_size = m_text_scale * p_font->getLineHeight();

    utils::Vector2f word_pos = m_page_position + Vec2{m_page_padding.x, -m_page_padding.y} - utils::Vector2f{0.f, m_line_size};
    auto drawWordAndMoveCursor = [&, this](Text &t_word)
    {
        auto b_box = t_word.getBoundingBox();
        if (word_pos.x + b_box.width > rightTextBorder()) //! line overflow -> newline
        {
            word_pos.x = leftTextBorder();
            word_pos.y -= m_line_size + m_line_spacing;
        }

        t_word.setPosition(word_pos);
        canvas.drawText2(t_word);
        word_pos.x += b_box.width + m_word_spacing;
    };

    //! iterate through words
    std::size_t start_pos = 0;
    std::size_t next_pos = m_text.find_first_of(' ');
    Text t_word = Text{m_text.substr(start_pos, next_pos - start_pos + 1)};
    t_word.setFont(p_font);
    t_word.setScale(m_text_scale, m_text_scale); //! fuck the flipping, fuck OpenGL coordinates, and fuck me
    t_word.m_edge_color = {255, 255, 255, 255};
    t_word.m_glow_color = {0, 0, 0, 0};

    while (next_pos != std::string::npos)
    {
        t_word.setText(m_text.substr(start_pos, next_pos - start_pos + 1));
        drawWordAndMoveCursor(t_word);

        start_pos = next_pos + 1;

        next_pos = m_text.find_first_of(' ', start_pos + 1);
        auto next_newline = m_text.find_first_of('\n', start_pos + 1);
        if (next_newline < next_pos)
        {
            t_word.setText(m_text.substr(start_pos, next_newline - start_pos));
            drawWordAndMoveCursor(t_word);

            word_pos.y -= m_line_size + m_line_spacing;
            word_pos.x = leftTextBorder();
            start_pos = next_newline + 1;
            next_pos = m_text.find_first_of(' ', start_pos + 1);
        }
    }

    //! draw the last word
    t_word.setText(m_text.substr(start_pos));
    drawWordAndMoveCursor(t_word);

    m_page_height = m_page_padding.y + m_line_size + m_line_spacing - (word_pos.y - m_page_position.y);
}

float Text::getCursorPosition(std::size_t cursor_pos)
{
    float pos = getPosition().x;
    for (std::size_t text_pos = 0; text_pos < cursor_pos; ++text_pos)
    {
        auto character = m_font->m_characters.at(m_text.at(text_pos));
        pos += (character.advance >> 6) * getScale().x;
    }
    return pos;
}

std::size_t Text::getCursor(float query_pos)
{

    float char_pos = getPosition().x;

    for (std::size_t text_pos = 0; text_pos < m_text.size(); ++text_pos)
    {
        if (query_pos <= char_pos)
        {
            return text_pos;
        }
        auto character = m_font->m_characters.at(m_text.at(text_pos));
        char_pos += (character.advance >> 6) * getScale().x;
    }

    return m_text.size();
}
MultiLineText::MultiLineText()
{
}

void MultiLineText::setText(std::string text)
{
    m_text = text;
}

void MultiLineText::setPosition(utils::Vector2f position)
{
    m_page_position = position;
}

utils::Vector2f MultiLineText::getPosition() const
{
    return m_page_position;
}

void MultiLineText::setPageWidth(float width)
{
    m_page_width = width;
}
float MultiLineText::getPageWidth() const
{
    return m_page_width;
}

float MultiLineText::getPageHeight() const
{
    return m_page_height;
}

void MultiLineText::setPadding(utils::Vector2f padding)
{
    m_page_padding = padding;
}

void MultiLineText::setWordSpacing(float spacing)
{
    m_word_spacing = spacing;
}

void MultiLineText::setLineSpacing(float spacing)
{
    m_line_spacing = spacing;
}

void MultiLineText::setScale(float scale)
{
    m_text_scale = scale;
}

void MultiLineText::setFont(Font *font)
{
    p_font = font;
}

float MultiLineText::leftTextBorder() const
{
    return m_page_position.x + m_page_padding.x;
}

float MultiLineText::rightTextBorder() const
{
    return m_page_position.x + m_page_width - m_page_padding.x;
}
