#pragma once

#include "Transform.h"
#include "Font.h"


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
    void setFont(Font* font);
    Font* getFont();
    void setText(const std::string &new_text);
    const std::string &getText() const;
    void setColor(ColorByte new_color);
    const ColorByte &getColor() const;


    float getTextWidth()const;
    void centerAround(const utils::Vector2f& center);

private:
    Font* m_font = nullptr;
    std::string m_text = "";
    ColorByte m_color = {255, 255, 255, 255};
};


// struct TextLineData
// {

// };