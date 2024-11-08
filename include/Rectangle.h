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
    GLuint m_vbo;

    Shader *m_shader = nullptr;
    static constexpr int m_indices[6] = {0, 1, 2, 2, 3, 0};

protected:
    Texture *m_texture = nullptr;
    std::array<Vertex, 4> m_verts;      
};



