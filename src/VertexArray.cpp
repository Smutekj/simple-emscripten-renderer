#include "VertexArray.h"

#include "Shader.h"
#include "View.h"

VertexArray::VertexArray(Shader &shader)
    : m_shader(&shader)
{
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glGenVertexArrays(1, &m_vao);
    glCheckError();

    m_textures.fill(0);
}

VertexArray::~VertexArray()
{
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_ebo);
    glDeleteVertexArrays(1, &m_vao);
}

void VertexArray::setTexture(int slot, GLuint texture_handle)
{
    m_textures.at(slot) = texture_handle;
}

VertexArray::VertexArray(Shader &shader, GLenum draw_type)
    : m_shader(&shader), m_draw_type(static_cast<DrawType>(draw_type))
{
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glGenVertexArrays(1, &m_vao);
    glCheckError();

    //! create proper VAO
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices.data(), draw_type);

    bindVertexAttributes(m_vbo, {2, 4, 2});

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

VertexArray::VertexArray(Shader &shader, GLenum draw_type, int n_verts)
    : VertexArray(shader, draw_type)
{
    resize(n_verts);
}

//! \brief resizes internal vetex array 
//! \brief in next draw call a glBufferData will be called creating a new buffer
//! \param n_verts  new maximum number of vertices
void VertexArray::resize(int n_verts)
{
    m_vertices.resize(n_verts);
    m_needs_new_gl_buffer = true;
}


//! \brief calls gl function to create vertex and element buffer buffers
void VertexArray::updateBufferData()
{
    
    if (!m_is_initialized) //! this way it's called just once for static draws
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glCheckError();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_vertices.size(), m_vertices.data());
        glCheckError();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    if (m_draw_type == DrawType::Static) //! for static draw we assume we do not need to change the data
    {
        m_is_initialized = true;
    }

}

//! \brief calls gl function to create vertex and element buffer buffers 
void VertexArray::createBuffers()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glCheckError();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(IndexType) * m_vertices.size(), NULL, static_cast<GLuint>(m_draw_type));
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glCheckError();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), static_cast<GLuint>(m_draw_type));
    glCheckError();

    bindVertexAttributes(m_vbo, {2, 4, 2});

    m_needs_new_gl_buffer = false;
    glBindVertexArray(0);
}

//! \brief does gl calls which initialize the array
//! \brief basically calls glBufferSubData
void VertexArray::initialize()
{
    if (m_needs_new_gl_buffer)
    {
        createBuffers();
    }
    else
    {
        updateBufferData();
    }
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

    for (int slot = 0; slot < N_MAX_TEXTURES_IN_SHADER; ++slot)
    {
        auto texture_id = m_textures.at(slot);
        if (texture_id != 0)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glCheckError();
        }
    }

    initialize();
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(IndexType) * indices.size(), indices.data());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glCheckError();
    
    glBindVertexArray(m_vao);
    glDrawElements(m_primitives, indices.size(), GL_UNSIGNED_SHORT, 0);
    glCheckError();

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
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

    for (int slot = 0; slot < N_MAX_TEXTURES_IN_SHADER; ++slot)
    {
        auto texture = m_textures.at(slot);
        if (texture != 0)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glBindTexture(GL_TEXTURE_2D, texture);
            glCheckError();
        }
    }
    initialize();

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

std::size_t VertexArray::size() const
{
    return m_vertices.size();
};

Vertex *VertexArray::data()
{
    return m_vertices.data();
}
