#pragma once

struct Color
{
    float r;
    float g;
    float b;
    float a;

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

struct ColorByte
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;
};
