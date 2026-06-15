#include "Rectangle.h"

#include "Shader.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "View.h"
#include "ViewMatrix.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>


static glm::mat4 getMatrix(const Transform &transform)
{
    glm::mat4 matrix;
    matrix = glm::translate(glm::mat4(1), glm::vec3(transform.getPosition().x, transform.getPosition().y, 0));
    matrix = glm::rotate(matrix, glm::radians(transform.getRotation()), glm::vec3(0, 0, 1));
    matrix = glm::scale(matrix, glm::vec3(transform.getScale().x, transform.getScale().y, 0));
    return matrix;
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

    shader.setUniform("u_view_projection", getMatrix(view));
    shader.setUniform("u_transform", getMatrix(*this));
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