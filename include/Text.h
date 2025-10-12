#pragma once

#include "Transform.h"
#include "Font.h"
#include "Rect.h"

//! \class Text
//! \brief contains necessary data to draw texts
//!  contains string to draw, color and pointer to font
//! when drawn, the characters are stored as distance fields, therefore
//!  effects like borders should be done using suitable shader.
//! \todo add italics/underscores/boldness,
//! \todo add possibility to define how to draw on multiple lines
class Text : public Transform
{

public:
    Text(std::string text = "");
    void setFont(Font *font);
    Font *getFont() const;
    void setText(const std::string &new_text);
    std::string getText() const;
    const std::wstring& getTextW() const
    {
        return m_text;
    }
    void setColor(ColorByte new_color);
    const ColorByte &getColor() const;

    Rect<float> getBoundingBox() const;
    float getDepthUnderLine() const;


    float getTextWidth() const;
    void centerAround(const utils::Vector2f &center);
    void centerAroundX(float center_x);
    void centerAroundY(float center_y);
public:
    bool m_draw_bounding_box = false;

private:
    Font *m_font = nullptr;
    std::wstring m_text = L"";
    ColorByte m_color = {255, 255, 255, 255};
};

class MultiLineText
{
public:
    MultiLineText()
    {
    }

    void drawInto(Renderer& canvas);

    void setText(std::string text)
    {
        m_text = text;
    }

    void setPosition(utils::Vector2f position)
    {
        m_page_position = position;
    }

    void setPageWidth(float width)
    {
        m_page_width = width;
    }
    float getPageWidth()const
    {
        return m_page_width;
    }

    float getPageHeight()const
    {
        return m_page_height;
    }

    void setPadding(utils::Vector2f padding)
    {
        m_page_padding = padding;
    }


    void setWordSpacing(float spacing)
    {
        m_word_spacing = spacing;
    }

    void setLineSpacing(float spacing)
    {
        m_line_spacing = spacing;
    }

    void setScale(float scale)
    {
        m_text_scale = scale;
    }

    void setFont(Font *font)
    {
        p_font = font;
    }

    float leftTextBorder() const
    {
        return m_page_position.x + m_page_padding.x;
    }

    float rightTextBorder() const
    {
        return m_page_position.x + m_page_width - m_page_padding.x;
    }

private:
    std::string m_text;

    Font *p_font = nullptr;

    utils::Vector2f m_page_padding;
    utils::Vector2f m_page_position;
    float m_line_size = 30; //! should be calculated from font probably
    float m_page_width = 600;
    float m_page_height; //! is calculated from text
    float m_line_spacing = 2;
    float m_word_spacing = 10;
    float m_text_scale = 1.;
    bool m_split_words = false;
};
