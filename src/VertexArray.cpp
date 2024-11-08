#include "VertexArray.h"

#include "Shader.h"
#include "View.h"

VertexArray::VertexArray(Shader &shader)
    : m_shader(&shader)
{
    gl::GenBuffers(1, &m_vbo);
    glCheckError();
    gl::GenBuffers(1, &m_ebo);
    glCheckError();

    m_textures.fill(0);
}

VertexArray::~VertexArray()
{
    gl::DeleteBuffers(1, &m_vbo);
    gl::DeleteBuffers(1, &m_ebo);
}

void VertexArray::setTexture(int slot, GLuint texture)
{
    m_textures.at(slot) = texture;
}

VertexArray::VertexArray(Shader &shader, GLenum draw_type)
    : m_shader(&shader), m_draw_type(static_cast<DrawType>(draw_type))
{
    gl::GenBuffers(1, &m_vbo);
    glCheckError();
    gl::GenBuffers(1, &m_ebo);
    glCheckError();
}

VertexArray::VertexArray(Shader &shader, GLenum draw_type, int n_verts)
    : VertexArray(shader, draw_type)
{
    resize(n_verts);
}

void VertexArray::resize(int n_verts)
{
    m_vertices.resize(n_verts);
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glCheckError();
    gl::BufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * n_verts, m_vertices.data(), static_cast<GLuint>(m_draw_type));
    glCheckError();
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glCheckError();
    gl::BufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(IndexType) * n_verts, NULL, static_cast<GLuint>(m_draw_type));
    glCheckError();
}

//! \brief does gl calls which initialize the array
//! \brief basically calls glBufferSubData
void VertexArray::init()
{
    gl::BindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glCheckError();

    bindVertexAttributes(m_vbo, {2, 4, 2});

    if (!m_is_initialized) //! this way it's called just once for static draws
    {
        gl::BufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());
        glCheckError();
    }

    if (m_draw_type == DrawType::Static) //! for static draw we assume we do not need to change the data
    {
        m_is_initialized = true;
    }

    gl::BindBuffer(GL_ARRAY_BUFFER, 0);
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//! \returns returns the \p i -th vertex in the array
Vertex &VertexArray::operator[](int i)
{
    return m_vertices.at(i);
}

//! \brief draws directly into the associated target
//! \param view
//! \param indices      a vector of indices to use in the draw call
void VertexArray::draw(View &view, const std::vector<IndexType> &indices)
{
    if (m_shader)
    {
        m_shader->use();
        m_shader->setMat4("u_view_projection", view.getMatrix());
        m_shader->activateTexture(m_textures);
        m_shader->setUniforms();
    }

    for (int slot = 0; slot < N_MAX_TEXTURES; ++slot)
    {
        auto texture_id = m_textures.at(slot);
        if (texture_id != 0)
        {
            gl::ActiveTexture(GL_TEXTURE0 + slot);
            gl::BindTexture(GL_TEXTURE_2D, texture_id);
            glCheckError();
        }
    }

    init();
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    gl::BufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(IndexType) * indices.size(), indices.data());
    glCheckError();

    gl::DrawElements(m_primitives, indices.size(), GL_UNSIGNED_SHORT, 0);
    glCheckError();

    gl::BindTexture(GL_TEXTURE_2D, 0);
    gl::BindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    gl::BindBuffer(GL_ARRAY_BUFFER, 0);
}

//! \brief draws directly into the associated target
//! \param view
void VertexArray::draw(View &view)
{
    if (m_shader)
    {
        m_shader->use();
        m_shader->setMat4("u_view_projection", view.getMatrix());
        m_shader->activateTexture(m_textures);
    }

    auto m = view.getMatrix();
    int i = 0;
    for (auto &v : m_vertices)
    {
        auto pos = m * glm::vec4(v.pos.x, v.pos.y, 0., 1.);
        std::cout << i << " " << pos.x << " " << pos.y << "\n";
        i++;
    }

    for (int slot = 0; slot < N_MAX_TEXTURES; ++slot)
    {
        auto texture = m_textures.at(slot);
        if (texture != 0)
        {
            gl::ActiveTexture(GL_TEXTURE0 + slot);
            gl::BindTexture(GL_TEXTURE_2D, texture);
            glCheckError();
        }
    }

    init();

    gl::DrawArrays(m_primitives, 0, m_vertices.size());
    glCheckError();

    gl::BindTexture(GL_TEXTURE_2D, 0);
    gl::BindBuffer(GL_ARRAY_BUFFER, 0);
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

