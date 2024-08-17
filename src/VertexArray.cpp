#include "VertexArray.h"

#include "Texture.h"
#include "Shader.h"
#include "View.h"

VertexArray::VertexArray(Shader &shader)
    : m_shader(&shader), m_shader2(shader)
{
    glGenBuffers(1, &m_vbo);
    glCheckError();
    glGenBuffers(1, &m_ebo);
    glCheckError();

    m_textures.fill(0);
}

void VertexArray::setTexture(int slot, GLuint texture)
{
    m_textures.at(slot) = texture;
}

VertexArray::VertexArray(Shader &shader, GLenum draw_type)
    : m_shader(&shader), m_shader2(shader), m_draw_type(draw_type)
{
    glGenBuffers(1, &m_vbo);
    glCheckError();
    glGenBuffers(1, &m_ebo);
    glCheckError();
}

VertexArray::VertexArray(Shader &shader, GLenum draw_type, int n_verts)
    : VertexArray(shader, draw_type)
{
    m_draw_type = draw_type;
    resize(n_verts);
}

void VertexArray::resize(int n_verts)
{
    m_vertices.resize(n_verts);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glCheckError();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * n_verts, m_vertices.data(), m_draw_type);
    glCheckError();
}

void VertexArray::init()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glCheckError();

    bindVertexAttributes(m_vbo, {2, 4, 2});

    if (!m_is_initialized)
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());
        glCheckError();
    }

    if (m_draw_type == GL_STATIC_DRAW) //! for static draw we assume we do not need to change the data
    {
        m_is_initialized = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Vertex &VertexArray::operator[](int i)
{
    return m_vertices.at(i);
}

void VertexArray::draw(View &view, const std::vector<IndexType> &indices)
{
    if (m_shader)
    {
        m_shader->use();
        m_shader->setMat4("u_view_projection", view.getMatrix());
        m_shader->activateTexture(0);
    }

    for (int slot = 0; slot < N_MAX_TEXTURES; ++slot)
    {
        auto texture_id = m_textures.at(slot);
        if (texture_id != 0)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glCheckError();
        }
    }

    init();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IndexType) * indices.size(), indices.data(), m_draw_type);
    glCheckError();

    glDrawElements(m_primitives, indices.size(), GL_UNSIGNED_SHORT, 0);
    glCheckError();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexArray::draw(View &view)
{
    if (m_shader)
    {
        m_shader->use();
        m_shader->setMat4("u_view_projection", view.getMatrix());
        m_shader->activateTexture(0);
    }

    for (int slot = 0; slot < N_MAX_TEXTURES; ++slot)
    {
        auto texture = m_textures.at(slot);
        if (texture != 0)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture);
            glCheckError();
        }
    }

    init();

    glDrawArrays(m_primitives, 0, m_vertices.size());
    glCheckError();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexArray::setTexture(Texture &texture)
{
    m_textures.at(0) = texture.getHandle();
    // setTexture(0, texture);
}

void VertexArray::setShader(Shader &shader)
{
    m_shader = &shader;
}

GLuint VertexArray::getShaderId() const
{
    return m_shader->getId();
}