#pragma once


//! \struct Rect
//! \brief represents an axis-aligned rectangle
//! position is defined by lower-left coordinate: pos_x, pos_y
//! and size is defined by width and height
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

using Rectf = Rect<float>;
using Recti = Rect<int>;
