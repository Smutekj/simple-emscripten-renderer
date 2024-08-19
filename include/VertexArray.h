#pragma once

#include "Vertex.h"
#include "IncludesGl.h"
#include "Shader.h"

#include <vector>
#include <memory>

// class Shader;
class Texture;
class View;

constexpr int N_MAX_TEXTURES = 2;

class VertexArray
{

public:
    // VertexArray();
    VertexArray(Shader &shader);
    VertexArray(Shader &shader, GLenum draw_type);
    VertexArray(Shader &shader, GLenum draw_type, int n_verts);
    ~VertexArray()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }

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
    GLenum m_primitives = GL_TRIANGLES;
    Shader* m_shader = nullptr;
    Shader& m_shader2;

private:

    GLuint m_vbo = -1;
    GLuint m_ebo = -1;

    bool m_is_initialized = false;

    std::array<GLuint, N_MAX_TEXTURES> m_textures = {0,0};

    std::vector<Vertex> m_vertices;

    GLenum m_draw_type = GL_DYNAMIC_DRAW;

};
