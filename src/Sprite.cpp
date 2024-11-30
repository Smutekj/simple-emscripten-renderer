#include "Sprite.h"

Sprite::Sprite(Texture *texture)
    : m_texture(texture)
{
    if (texture)
    {
        m_tex_rect = {0, 0, (int)texture->getSize().x, (int)texture->getSize().y};
        m_texture_handles.at(0) = texture->getHandle();
        m_tex_size = texture->getSize();
    }
}
Sprite::Sprite(Texture &texture)
    : m_texture(&texture),
      m_tex_rect(0, 0, (int)texture.getSize().x, (int)texture.getSize().y)
{
    m_texture_handles.at(0) = texture.getHandle();
    m_tex_size = texture.getSize();
}

void Sprite::setTexture(Texture &texture)
{
    setTexture(0, texture);
}

void Sprite::setTexture(int slot, Texture &texture)
{
    m_texture = &texture;
    m_texture_handles.at(slot) = texture.getHandle();
    m_tex_rect = {0, 0, (int)texture.getSize().x, (int)texture.getSize().y};
    m_tex_size = texture.getSize();
}

void Sprite::setTextureP(int slot, Texture *texture)
{
    if (!texture)
    {
        return;
    }
    m_texture = texture;
    m_texture_handles.at(slot) = texture->getHandle();
    m_tex_rect = {0, 0, (int)texture->getSize().x, (int)texture->getSize().y};
    m_tex_size = texture->getSize();
}

const ColorByte &RectangleSimple::getColor() const
{
    return m_color;
}

void RectangleSimple::setColor(ColorByte &color)
{
    m_color = color;
}
