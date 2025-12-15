#include "Color.h"

Color::Color(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a)
{
}

Color::Color(const ColorByte &col)
    : r(static_cast<float>(col.r / 255.f)),
      g(static_cast<float>(col.g / 255.f)),
      b(static_cast<float>(col.b / 255.f)),
      a(static_cast<float>(col.a / 255.f))
{
}

Color Color::operator+(const Color &other) const
{
    return {r + other.r, g + other.g, b + other.b, a + other.a};
}

Color Color::operator-(const Color &other) const
{
    return {r - other.r, g - other.g, b - other.b, a - other.a};
}

Color Color::operator*(const Color &other) const
{
    return {r * other.r, g * other.g, b * other.b, a * other.a};
}
Color Color::operator*(float scalar) const
{
    return {r * scalar, g * scalar, b * scalar, a * scalar};
}

Color Color::operator/(float scalar) const
{
    return {r / scalar, g / scalar, b / scalar, a / scalar};
}

ColorByte::ColorByte(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    : r(r), g(g), b(b), a(a)
{
}
ColorByte::ColorByte(const Color &col)
    : r(static_cast<unsigned char>(col.r * 255)),
      g(static_cast<unsigned char>(col.g * 255)),
      b(static_cast<unsigned char>(col.b * 255)),
      a(static_cast<unsigned char>(col.a * 255))
{
}

bool ColorByte::operator==(const ColorByte &other) const
{
    return r == other.r && b == other.b && g == other.g && a == other.a;
}