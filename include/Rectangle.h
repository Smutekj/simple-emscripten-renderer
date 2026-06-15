#pragma once

#include "Rect.h"
#include "Transform.h"
#include "IncludesGl.h"
#include "Vertex.h"
#include "VertexArray.h"

class Texture;
class Shader;
class View;


class RenderTarget;
//! \class DrawSprite
//! \brief represents a rectangle, which holds it's vertices and all draw info like textures/shader/color
class DrawSprite : public Transform
{

public:
    DrawSprite();
    ~DrawSprite();

    void draw(RenderTarget &target, Shader &shader, TextureArray textures, View &view);

    GLuint getVAO()const;

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};

//! \brief Sprite that is used for drawing on the entire target.
class ScreenSprite
{
public:
    void draw(RenderTarget &target, Shader &shader, TextureArray texture_handles);
    void draw(RenderTarget &target, Shader &shader, const Texture &source);
    void draw(RenderTarget &target, Shader &shader, const Texture &source, const Texture &source2);
    template <class... Textures>
    void draw(RenderTarget &target, Shader &shader, const Textures &...sources);

    Rectf tex_rect;
private:
    DrawSprite m_screen_sprite;
};