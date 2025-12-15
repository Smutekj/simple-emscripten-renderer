#pragma once

#include "Transform.h"
#include "Rect.h"

#include <array>
#include "GLTypeDefs.h"
#include "Color.h"

class Texture;

//! \struct RectangleSimple
//! \brief a transform with width and height, which represents a rectangle
struct RectangleSimple : public Transform
{
    RectangleSimple(Color color = {1,1,1,1});

    Color m_color;
};

//! \struct Sprite
//! \brief holds data regarding Transform world-size and knows, what textures the Sprite uses
struct Sprite : public Transform 
{
    Sprite() = default;
    explicit Sprite(const Texture &texture);

    void setColor(ColorByte color);

    void setTexture(const Texture &texture);
    void setTexture(GLuint id, int slot = 0);
    void setTexture(int slot, const Texture &texture);

    ColorByte m_color = {255, 255, 255, 255};
    TextureArray m_texture_handles = {}; //!< GL handles of the bound textures
    utils::Vector2i m_tex_size = {1, 1};        //!< ???
    Rect<int> m_tex_rect = {0,0,1,1};          //!< defines part of the texture that will be drawn
};
