#pragma once

#include "Rect.h"
#include "Transform.h"
#include "IncludesGl.h"
#include "Vertex.h"
#include "VertexArray.h"

class Texture;
class Shader;
class View;

class Rectangle : public Transform
{

public:
    Rectangle(Shader &shader);
    ~Rectangle()
    {
        glDeleteBuffers(1, &m_vbo);
        glCheckError();
    }
    void initialize();

    void draw(GLuint target, View &view);

    void setShader(Shader &shader);

    void setTexture(Texture &texture);

    void setColor(Color color);
    void setTexCoords(float tex_coord_x, float tex_coord_y, float width, float height);
    std::vector<Vertex> getVerts();
private:
    GLuint m_vbo;

    Shader *m_shader = nullptr;
    static constexpr int m_indices[6] = {0, 1, 2, 2, 3, 0};

protected:
    Texture *m_texture = nullptr;
    std::vector<Vertex> m_verts;
    // VertexArray m_verts;
};





class Sprite : public Rectangle
{

public:
    Sprite(Texture &texture, Shader &shader);
    void setTextureRect(Rect<int> tex_rect);

private:
    // Texture &m_texture;
    Rect<float> m_tex_rect = {0, 0, 1, 1};
};