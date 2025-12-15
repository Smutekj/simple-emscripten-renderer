#pragma once

#include "Utils/Vector2.h"

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

    bool inline intersects(const Rect<T> &r2) const
    {
        auto min_x = pos_x;
        auto max_x = pos_x + width;
        auto min_y = pos_y;
        auto max_y = pos_y + height;
    
        auto min_x_other = r2.pos_x;
        auto max_x_other = r2.pos_x + r2.width;
        auto min_y_other = r2.pos_y;
        auto max_y_other = r2.pos_y + r2.height;

        bool intersects_x = min_x <= max_x_other && max_x >= min_x_other;
        bool intersects_y = min_y <= max_y_other && max_y >= min_y_other;
        return intersects_x && intersects_y;
    }

    template <class QueryType>
    bool inline contains(const utils::Vector2<QueryType> &query) const
    {
        return query.x >= pos_x && query.x <= pos_x + width &&
               query.y >= pos_y && query.y <= pos_y + height;
    }
};

using Rectf = Rect<float>;
using Recti = Rect<int>;
