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

    bool inline intersects(const Rect<T> &r2)
    {
        auto min_x = pos_x - width/2;
        auto max_x = pos_x + width/2;
        auto min_y = pos_y - height/2;
        auto max_y = pos_y + height/2;
    {
        auto min_x_other = r2.pos_x - r2.width/2;
        auto max_x_other = r2.pos_x + r2.width/2;
        auto min_y_other = r2.pos_y - r2.height/2;
        auto max_y_other = r2.pos_y + r2.height/2;

        bool intersects_x = min.x <= max_x_other && max_x >= min_x_other;
        bool intersects_x = min.y <= max_y_other && max_y >= min_y_other;
        return intersects_x && intersects_y;
    }
};

using Rectf = Rect<float>;
using Recti = Rect<int>;
