#include "Sprite.h"
#include "Texture.h"

Sprite::Sprite(const Texture &texture)
    : m_tex_rect(0, 0, (int)texture.getSize().x, (int)texture.getSize().y)
{
    m_texture_handles.at(0) = texture.getHandle();
    m_tex_size = texture.getSize();
}

void Sprite::setTexture(GLuint tex_id, int slot)
{
    m_texture_handles.at(slot) = tex_id;
}
void Sprite::setTexture(const Texture &texture)
{
    setTexture(0, texture);
}

void Sprite::setTexture(int slot, const Texture &texture)
{
    m_texture_handles.at(slot) = texture.getHandle();
    m_tex_rect = {0, 0, (int)texture.getSize().x, (int)texture.getSize().y};
    m_tex_size = texture.getSize();
}

void Sprite::setColor(ColorByte color)
{
    m_color = color;
}

RectangleSimple::RectangleSimple(Color color)
    : m_color(color)
{
}