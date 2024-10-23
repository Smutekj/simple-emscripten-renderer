#include "Rectangle.h"

#include "Shader.h"
#include "Texture.h"
#include "View.h"

DrawRectangle::DrawRectangle(Shader &shader) : m_shader(&shader)
{
    initialize();
}

DrawRectangle::~DrawRectangle()
{
    glDeleteBuffers(1, &m_vbo);
    glCheckError();
}

//! \brief initializes a vertex buffer
void DrawRectangle::initialize()
{

    m_verts[0] = {{-1.f, -1.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 1.f}};
    m_verts[1] = {{1.f, -1.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 1.f}};
    m_verts[2] = {{1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {1.f, 0.f}};
    m_verts[3] = {{-1.f, 1.f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 0.f}};

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_verts.size(), m_verts.data(), GL_STATIC_DRAW);
    glCheckError();
}


//! \brief draws into a target spec using the \p view
//! \param terget       GL handle of the render target
//! \param view         tells us what part of the world is drawn.
void DrawRectangle::draw(GLuint target, View &view)
{

    glBindFramebuffer(GL_FRAMEBUFFER, target);

    m_shader->use();
    m_shader->setMat4("u_view_projection", view.getMatrix());
    m_shader->setMat4("u_transform", getMatrix());

    if (m_texture)
    {
        m_shader->setInt("u_texture", 0);
        m_texture->bind();
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glCheckError();
    bindVertexAttributes(m_vbo, {2, 4, 2});
    // glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_verts.size(), m_verts.data());
    glCheckError();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, m_indices);
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void DrawRectangle::setShader(Shader &shader)
{
    m_shader = &shader;
}

void DrawRectangle::setTexture(Texture &texture)
{
    m_texture = &texture;
}

void DrawRectangle::setColor(Color color)
{
    for (int i = 0; i < m_verts.size(); ++i)
    {
        m_verts[i].color = color;
    }
}

void DrawRectangle::setTexCoords(float tex_coord_x, float tex_coord_y, float width, float height)
{

    m_verts[0].tex_coord = {tex_coord_x, tex_coord_y};
    m_verts[1].tex_coord = {tex_coord_x, tex_coord_y};
    m_verts[2].tex_coord = {tex_coord_x, tex_coord_y};
    m_verts[3].tex_coord = {tex_coord_x, tex_coord_y};
}

std::vector<Vertex> DrawRectangle::getVerts()
{
    auto m = getMatrix();
    std::vector<Vertex> new_verts;
    new_verts.resize(m_verts.size());
    for (std::size_t i = 0; i < m_verts.size(); ++i)
    {
        glm::vec4 v(m_verts[i].pos.x, m_verts[i].pos.y, 0, 1);
        auto v_transformed = m * v;
        new_verts[i] = m_verts[i];
        new_verts[i].pos = {v_transformed.x, v_transformed.y};
    }

    return new_verts;
}

// Sprite::Sprite(Texture &texture, Shader &shader)
//     : Rectangle(shader)
// {
//     setTexture(texture);
// }

// void Sprite::setTextureRect(Rect<int> tex_rect)
// {
//     auto tex_size = m_texture->getSize();
//     m_tex_rect = {tex_rect.pos_x / tex_size.x, tex_rect.pos_y / tex_size.y,
//                   tex_rect.width / tex_size.x, tex_rect.height / tex_size.y};

//     m_verts[0].tex_coord = {m_tex_rect.pos_x, m_tex_rect.pos_y};
//     m_verts[1].tex_coord = {m_tex_rect.pos_x + m_tex_rect.width, m_tex_rect.pos_y};
//     m_verts[2].tex_coord = {m_tex_rect.pos_x + m_tex_rect.width, m_tex_rect.pos_y + m_tex_rect.height};
//     m_verts[3].tex_coord = {m_tex_rect.pos_x, m_tex_rect.pos_y + m_tex_rect.height};
// }
