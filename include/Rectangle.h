#pragma once

#include "Rect.h"
#include "Transform.h"
#include "IncludesGl.h"
#include "Vertex.h"
#include "VertexArray.h"

class Texture;
class Shader;
class View;

//! \class DrawRectangle
//! \brief represents a rectangle, which holds it's vertices and all draw info like textures/shader/color
class DrawRectangle : public Transform
{

public:
    DrawRectangle(Shader &shader);
    ~DrawRectangle();

    void initialize();

    void draw(GLuint target, View &view);

    void setShader(Shader &shader);
    void setTexture(Texture &texture);
    void setColor(Color color);
    void setTexCoords(float tex_coord_x, float tex_coord_y, float width, float height);

    std::vector<Vertex> getVerts();

private:
    GLuint m_vbo = 0;

    Shader *m_shader = nullptr;
    static constexpr int m_indices[6] = {0, 1, 2, 2, 3, 0};

protected:
    Texture *m_texture = nullptr;
    std::array<Vertex, 4> m_verts;
};

class RenderTarget;

//! \class DrawSprite
//! \brief represents a rectangle, which holds it's vertices and all draw info like textures/shader/color
class DrawSprite : public Transform
{

public:
    DrawSprite();
    ~DrawSprite();

    void draw(RenderTarget &target, Shader &shader, TextureArray textures, View &view);

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};

class ScreenSprite
{
public:
    void draw(RenderTarget &target, Shader &shader, const Texture &source);
    void draw(RenderTarget &target, Shader &shader, const Texture &source, const Texture &source2);
    template <class... Textures>
    void draw(RenderTarget &target, Shader &shader, const Textures &...sources);

private:
    DrawSprite m_screen_sprite;
};