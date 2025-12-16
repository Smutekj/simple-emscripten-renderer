#include "Rectangle.h"

#include "Shader.h"
#include "Texture.h"
#include "RenderTarget.h"
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

    m_shader->setUniform("u_view_projection", view.getMatrix());
    m_shader->setUniform("u_transform", getMatrix());
    m_shader->use();

    if (m_texture)
    {
        m_texture->bind(0);
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
    for (auto &vertex : m_verts)
    {
        vertex.color = color;
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

DrawSprite::~DrawSprite()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

DrawSprite::DrawSprite()
{

    static constexpr float VERTEX_RECT[6 * 4] = {
        -1, -1, 0, 0,
        -1, +1, 0, 1,
        +1, -1, 1, 0,
        +1, +1, 1, 1,
        +1, -1, 1, 0,
        -1, +1, 0, 1};

    glGenBuffers(1, &m_vbo);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTEX_RECT), (void *)VERTEX_RECT, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void *)(0 * sizeof(float)));
    glVertexAttribDivisor(0, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void *)(2 * sizeof(float)));
    glVertexAttribDivisor(1, 0);
    glCheckError();

    glBindVertexArray(0);
}
GLuint DrawSprite::getVAO() const
{
    return m_vao;
}

void DrawSprite::draw(RenderTarget &target, Shader &shader, TextureArray textures, View &view)
{

    target.bind();

    shader.setUniform("u_view_projection", view.getMatrix());
    shader.setUniform("u_transform", getMatrix());
    shader.use();

    for (int tex_id = 0; tex_id < textures.size(); ++tex_id)
    {
        if (textures[tex_id] != 0)
        {
            glActiveTexture(GL_TEXTURE0 + tex_id);
            glBindTexture(GL_TEXTURE_2D, textures[tex_id]);
            glCheckError();
        }
    }

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glCheckError();

    glBindVertexArray(0);
}

void ScreenSprite::draw(RenderTarget &target, Shader &shader, TextureArray texture_handles)
{
    glViewport(0, 0, target.getSize().x, target.getSize().y);
    target.bind();
    shader.use();

    for (int tex_id = 0; tex_id < texture_handles.size(); ++tex_id)
    {
        if (texture_handles[tex_id] != 0)
        {
            glActiveTexture(GL_TEXTURE0 + tex_id);
            glBindTexture(GL_TEXTURE_2D, texture_handles[tex_id]);
            glCheckError();
        }
    }

    glBindVertexArray(m_screen_sprite.getVAO());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glCheckError();

    glBindVertexArray(0);
}

void ScreenSprite::draw(RenderTarget &target, Shader &shader, const Texture &source)
{
    draw(target, shader, TextureArray{source.getHandle(), 0});
}

template <class... Textures>
void ScreenSprite::draw(RenderTarget &target, Shader &shader, const Textures &...sources)
{
    static_assert((std::is_same_v<Textures, Texture> && ...),
                  "All types of sources must have type Texture");
    // Collect texture handles
    std::array<TextureHandle, sizeof...(sources)> tex_array{
        sources.getHandle()...};

    draw(target, shader, tex_array);
}

void ScreenSprite::draw(RenderTarget &target, Shader &shader, const Texture &source, const Texture &source2)
{
    draw(target, shader, TextureArray{source.getHandle(), source2.getHandle()});
}