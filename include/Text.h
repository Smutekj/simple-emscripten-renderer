#pragma once


#include "Transform.h"

class Font;

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

private:
    Font* m_font = nullptr;
    std::string m_text = "";
    ColorByte m_color = {255, 255, 255, 255};
};
