#pragma once

#include "Transform.h"
#include "Texture.h"
#include "Rect.h"

#include <array>

//! \struct RectangleSimple
//! \brief a transform with width and height, which represents a rectangle
struct RectangleSimple : public Transform
{
    float width = 1;
    float height = 1;
};

//! \struct Sprite
//! \brief holds data regarding Transform world-size and knows, what textures the Sprite uses
struct Sprite : public RectangleSimple
{

    Sprite(Texture *texture = nullptr);
    Sprite(Texture &texture);

    void setTexture(Texture &texture);

    void setTexture(int slot, Texture &texture);
    void setTextureP(int slot, Texture *texture = nullptr);

    const ColorByte &getColor() const;
    void setColor(ColorByte &color);

    Texture *m_texture = nullptr;
    std::array<TextureHandle, N_MAX_TEXTURES> m_texture_handles = {0, 0}; //!< GL handles of the bound textures
    utils::Vector2i m_tex_size = {0, 0};                                  //!< ???
    Rect<int> m_tex_rect;                                                 //!< defines part of the texture that will be drawn
    ColorByte m_color = {255, 255, 255, 255};                             //!< can be used in shaders
};
