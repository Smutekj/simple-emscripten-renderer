#pragma once

#include "Utils/Vector2.h"
#include "Color.h"

using Vec2 = utils::Vector2f;

//! \struct Vertex
//! \brief Vertex data sent into shaders
struct Vertex
{
    Vec2 pos;
    Color color;
    Vec2 tex_coord;
};

