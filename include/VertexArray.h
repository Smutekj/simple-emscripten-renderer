#pragma once

#include "Vertex.h"
#include "IncludesGl.h"
#include "Shader.h"
#include "Texture.h"

#include <vector>
#include <memory>


enum class DrawType
{
    Dynamic = GL_DYNAMIC_DRAW,
    Static = GL_STATIC_DRAW,
    Stream = GL_STREAM_DRAW, 
};


class View;


//! \class holds vertices for drawing and does all the OpenGL stuff
class VertexArray
{

public:
    VertexArray(Shader &shader);
    VertexArray(Shader &shader, GLenum draw_type);
    VertexArray(Shader &shader, GLenum draw_type, int n_verts);
    ~VertexArray();

    void resize(int n_verts);

    void init();

    void draw(View &view);

    void draw(View& view, const std::vector<IndexType>& indices);

    void setTexture(Texture &texture);
    void setTexture(int slot, GLuint);

    void setShader(Shader &Shader);

    std::size_t size() const
    {
        return m_vertices.size();
    };

    Vertex &operator[](int i);

    Vertex* data()
    {
        return m_vertices.data();
    }

    GLuint getShaderId()const;

public:     
    GLenum m_primitives = GL_TRIANGLES; //! 
    Shader* m_shader = nullptr;         //! pointer to shader

private:

    GLuint m_vbo = -1;      //! vertex buffer object OpenGL id
    GLuint m_ebo = -1;      //! element buffer object OpenGL id

    bool m_is_initialized = false;

    std::array<GLuint, N_MAX_TEXTURES> m_textures = {0,0};

    std::vector<Vertex> m_vertices;

    DrawType m_draw_type = DrawType::Dynamic;

};


void inline bindVertexAttributes(GLuint buffer, std::vector<int> sizes);
