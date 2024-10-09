#pragma once

#include "Transform.h"
#include "Texture.h"
#include "Rect.h"

#include <array>



struct RectangleSimple : public Transform
{
    float width = 1;
    float height = 1;
};

struct Sprite : public RectangleSimple
{

    Sprite(Texture *texture = nullptr);
    Sprite(Texture &texture);

    void setTexture(Texture &texture);

    void setTexture(int slot, Texture &texture);
    void setTextureP(int slot, Texture *texture = nullptr);

    Texture *m_texture = nullptr;
    std::array<TextureHandle, N_MAX_TEXTURES> m_texture_handles = {0, 0};
    utils::Vector2i m_tex_size = {0, 0};
    Rect<int> m_tex_rect;
    ColorByte m_color = {255, 255, 255, 255};
};
