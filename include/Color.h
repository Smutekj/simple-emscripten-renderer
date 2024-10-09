#pragma once


//! \struct Color, where each channel is a 32-bit float
struct Color
{
    float r = 0.;
    float g = 0.;
    float b = 0.;
    float a = 0.;

    Color operator+(const Color &other) const
    {
        return {r + other.r, g + other.g, b + other.b, a + other.a};
    }

    Color operator-(const Color &other) const
    {
        return {r - other.r, g - other.g, b - other.b, a - other.a};
    }

    Color operator*(const Color &other) const
    {
        return {r * other.r, g * other.g, b * other.b, a * other.a};
    }
    Color operator*(float scalar) const
    {
        return {r * scalar, g * scalar, b * scalar, a * scalar};
    }
};

//! \struct Color, where each channel is a 8-bit unsigned number (0-255)
struct ColorByte
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    ColorByte() = default;
    ColorByte(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
        : r(r), g(g), b(b), a(a)
    {
    }
    ColorByte(const Color &col)
        : r(static_cast<unsigned char>(col.r * 255)),
          g(static_cast<unsigned char>(col.g * 255)),
          b(static_cast<unsigned char>(col.b * 255)),
          a(static_cast<unsigned char>(col.a * 255))
    {}
};
