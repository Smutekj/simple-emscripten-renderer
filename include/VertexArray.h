#pragma once

#include "Vertex.h"
#include "Shader.h"
#include "Texture.h"

#include <vector>
#include <memory>

#include "GLTypeDefs.h"

class View;

//! \class VertexArray
//! \brief holds vertices for drawing and does all the OpenGL stuff
class VertexArray
{

public:
    VertexArray();
    VertexArray(DrawType draw_type);
    VertexArray(DrawType draw_type, int n_verts);
    ~VertexArray();

    void resize(int n_verts);
    std::size_t size() const;

    void draw(View view, Shader &shader);
    void draw(View view, Shader &shader, const std::vector<IndexType> &indices);

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

    std::vector<Vertex> m_vertices;
private:
    GLuint m_vbo = -1; //<! vertex buffer object OpenGL id
    GLuint m_ebo = -1; //<! element buffer object OpenGL id
    GLuint m_vao = -1; //<! vertex array object OpenGL id

    TextureArray m_textures = {};

    DrawType m_draw_type = DrawType::Dynamic;

    bool m_is_initialized = false;
    bool m_needs_new_gl_buffer = false;
};

class RenderTarget;
//! \class VertexArray
//! \brief holds vertices for drawing and does all the OpenGL stuff
class VertexArrayIndexed
{

public:
    VertexArrayIndexed();
    VertexArrayIndexed(DrawType draw_type);
    VertexArrayIndexed(DrawType draw_type, int n_verts, int n_inds);
    ~VertexArrayIndexed();

    void resize(int n_verts);
    std::size_t size() const;

    void draw(RenderTarget& target, View view, Shader &shader, TextureArray textures);

    Vertex &operator[](int i);
    Vertex *data();

private:
    void createBuffers();
    void updateBufferData(int max_vertex_ind = -1);
    void initialize();

public:
    GLenum m_primitives = GL_TRIANGLES; //!

    std::vector<Vertex> m_vertices;
    std::vector<int> m_indices;
private:
    GLuint m_vbo = -1; //<! vertex buffer object OpenGL id
    GLuint m_ebo = -1; //<! element buffer object OpenGL id
    GLuint m_vao = -1; //<! vertex array object OpenGL id

    TextureArray m_textures = {};

    DrawType m_draw_type = DrawType::Dynamic;

    bool m_is_initialized = false;
    bool m_needs_new_gl_buffer = false;
};

