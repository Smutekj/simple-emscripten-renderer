#pragma once

struct ColorByte;
//! \struct Color
//! \brief  each channel is a 32-bit float
struct Color
{
    Color() = default;
    explicit Color(const ColorByte &col);
    Color(float r, float g, float b, float a = 1.);

    Color operator+(const Color &other) const;
    Color operator-(const Color &other) const;
    Color operator*(const Color &other) const;
    Color operator*(float scalar) const;
    Color operator/(float scalar) const;

    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
    float a = 0.f;
};

Color operator*(float scalar, const Color &other);
Color operator/(float scalar, const Color &other);

//! \struct ColorByte,
//! \brief  each channel is a 8-bit unsigned number (0-255)
struct ColorByte
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    ColorByte() = default;
    ColorByte(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    explicit ColorByte(const Color &col);

    bool operator==(const ColorByte &other) const;
};
