#pragma once

#include "Vertex.h"
#include "IncludesGl.h"
#include "Shader.h"
#include "Texture.h"

#include <vector>
#include <memory>

//! \enum DrawType
//! \brief Corresponds to OpenGL draw buffers needed in glBufferData
enum class DrawType
{
    Dynamic = GL_DYNAMIC_DRAW,
    Static = GL_STATIC_DRAW,
    Stream = GL_STREAM_DRAW,
};

class View;

//! \class VertexArray
//! \brief holds vertices for drawing and does all the OpenGL stuff
class VertexArray
{

public:
    VertexArray();
    VertexArray(GLenum draw_type);
    VertexArray(GLenum draw_type, int n_verts);
    ~VertexArray();

    void resize(int n_verts);
    std::size_t size() const;

    void draw(View &view, Shader &shader);
    void draw(View &view, Shader &shader, const std::vector<IndexType> &indices);

    void setTexture(Texture &texture);
    void setTexture(int slot, GLuint);

    Vertex &operator[](int i);

    Vertex *data();

private:
    void createBuffers();
    void updateBufferData(int max_vertex_ind = -1);
    void initialize();

public:
    GLenum m_primitives = GL_TRIANGLES; //!

private:
    GLuint m_vbo = -1; //<! vertex buffer object OpenGL id
    GLuint m_ebo = -1; //<! element buffer object OpenGL id
    GLuint m_vao = -1; //<! vertex array object OpenGL id

    TextureArray m_textures = {};

    std::vector<Vertex> m_vertices;

    DrawType m_draw_type = DrawType::Dynamic;

    bool m_is_initialized = false;
    bool m_needs_new_gl_buffer = false;
};
