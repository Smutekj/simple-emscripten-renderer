#pragma once

template <class T>
struct Rect
{
    T pos_x;
    T pos_y;
    T width;
    T height;

    Rect() = default;
    Rect(T x, T y, T w, T h)
        : pos_x(x), pos_y(y), width(w), height(h)
    {
    }
};
