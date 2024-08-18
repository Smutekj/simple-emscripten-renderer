#pragma once

struct Vec2
{
    float x;
    float y;
};

struct Color
{
    float r;
    float g;
    float b;
    float a;

    Color operator+(const Color& other) const
    {
        return {r + other.r, g + other.g, b + other.b, a + other.a};
    }

    Color operator-(const Color& other) const
    {
        return {r - other.r, g - other.g, b - other.b, a - other.a};
    }

    Color operator*(const Color& other) const
    {
        return {r*other.r, g*other.g, b*other.b, a*other.a};
    }
    Color operator*(float scalar) const
    {
        return {r*scalar, g*scalar, b*scalar, a*scalar};
    }
};

struct Vertex
{
    Vec2 pos;
    Color color;
    Vec2 tex_coord;
    // 
};
