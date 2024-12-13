#pragma once

#include <random>

#include "Vector2.h"

inline float randf(const float min = 0, const float max = 1)
{
    return (rand() / static_cast<float>(RAND_MAX)) * (max - min) + min;
}

inline Vec2 randomPosInBox(utils::Vector2f ul_corner,
                           utils::Vector2f box_size)
{
    return {ul_corner.x + rand() / static_cast<float>(RAND_MAX) * box_size.x,
            ul_corner.y + rand() / static_cast<float>(RAND_MAX) * box_size.y};
}
