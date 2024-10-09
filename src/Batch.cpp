#include "Batch.h"

Batch::Batch(GLuint texture_id, Shader &shader, DrawType draw_type)
    : m_config(texture_id, shader.getId(), draw_type),
      m_verts(shader, static_cast<GLenum>(draw_type), m_capacity)
{
}
Batch::Batch(const BatchConfig &config, Shader &shader, DrawType draw_type)
    : m_config(config),
      m_verts(shader, static_cast<GLenum>(draw_type), m_capacity)
{
    m_indices.reserve(m_capacity * 3);
    std::for_each(config.texture_ids.begin(), config.texture_ids.end(), [&config, this](auto &id)
                  {
        int slot = &id - config.texture_ids.begin();
        m_verts.setTexture(slot, m_config.texture_ids.at(slot)); });
}

void Batch::clear()
{
    m_used_vertices = 0;
    m_indices.clear();
}

void Batch::flush(View &view)
{

    m_verts.draw(view, m_indices);

    if (m_config.draw_type != DrawType::Static) //! static draws should stay the same so no need to clear data
    {
        clear();
    }
}

void Batch::pushVertexArray(std::vector<Vertex> &verts)
{
    for (auto &v : verts)
    {
        pushVertex(v);
    }
}

void Batch::pushVertex(Vertex v)
{
    m_verts[m_used_vertices] = v;
    m_indices.push_back(m_used_vertices);
    m_used_vertices++;
    assert(m_used_vertices <= m_capacity);
}

void Batch::pushVertex(int ind)
{
    m_indices.push_back(ind);
}

BatchConfig Batch::getConfig() const
{
    return m_config;
}

BatchConfig::BatchConfig(std::array<GLuint, N_MAX_TEXTURES> tex_ids, const GLuint &shader_id, DrawType draw_type)
    : shader_id(shader_id), draw_type(draw_type)
{
    assert(tex_ids.size() <= N_MAX_TEXTURES);
    std::copy(tex_ids.begin(), tex_ids.end(), texture_ids.begin());
}
BatchConfig::BatchConfig(const GLuint &tex_id, const GLuint &shader_id, DrawType draw_type)
    : shader_id(shader_id), draw_type(draw_type)
{
    texture_ids[0] = tex_id;
}

bool BatchConfig::operator==(const BatchConfig &other) const
{
    bool shaders_same = other.shader_id == shader_id;
    bool textures_same = std::equal(texture_ids.begin(), texture_ids.end(), std::begin(other.texture_ids));
    bool drawtypes_same = draw_type == other.draw_type;
    return shaders_same && textures_same && drawtypes_same;
}

int Batch::getFreeVerts() const
{
    return m_capacity - m_used_vertices;
}

void SpriteBatch::createBuffers()
{
    glGenBuffers(1, &m_indices_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6, m_indices, GL_STATIC_DRAW);
    glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * 4, m_prototype, GL_STATIC_DRAW);
    glCheckError();

    glGenBuffers(1, &m_transform_buffer);
    glCheckError();
    glBindBuffer(GL_ARRAY_BUFFER, m_transform_buffer);
    glCheckError();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Trans) * BATCH_VERTEX_CAPACITY, m_transforms.data(), GL_DYNAMIC_DRAW);
    glCheckError();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SpriteBatch::SpriteBatch(BatchConfig config, Shader &shader)
    : m_config(config), m_shader(shader)
{
    createBuffers();
}

SpriteBatch::SpriteBatch(GLuint texture_id, Shader &shader)
    : m_shader(shader), m_texture_id(texture_id)
{
    createBuffers();
}

bool SpriteBatch::addSprite(Trans t)
{
    if (m_end >= 2000)
    {
        return false;
    }
    m_transforms.at(m_end) = t;
    m_end++;
    return true;
};

//! TODO: I should probably start using VAOs... since we can use GLES 3.0
void SpriteBatch::bindAttributes()
{

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), (void *)(0 * sizeof(float)));
    glVertexAttribDivisor(0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_transform_buffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Trans), (void *)(0 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Trans), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Trans), (void *)(4 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Trans), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(Trans), (void *)(7 * sizeof(float)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Trans), (void *)(9 * sizeof(float)));

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glCheckError();
}

void SpriteBatch::initialize()
{

    glBindBuffer(GL_ARRAY_BUFFER, m_transform_buffer);
    glCheckError();
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Trans) * m_end, m_transforms.data());
    glCheckError();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glCheckError();

    bindAttributes();
    glCheckError();
}

int SpriteBatch::countFreeSpots() const
{
    return m_transforms.size() - m_end;
}

void SpriteBatch::flush(View &view)
{
    m_shader.use();
    m_shader.setMat4("u_view_projection", view.getMatrix());
    m_shader.setUniforms();
    m_shader.activateTexture(m_config.texture_ids);

    for (int slot = 0; slot < m_config.texture_ids.size(); ++slot)
    {
        auto texture_id = m_config.texture_ids.at(slot);
        if (texture_id != 0)
        {
            glActiveTexture(GL_TEXTURE0 + slot);
            glCheckError();
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glCheckError();
        }
    }
    initialize();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices_buffer);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, m_end);
    glCheckError();

    //! reset instance count
    m_end = 0;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int SpriteBatch::getFreeVerts() const
{
    return m_transforms.size() - m_end;
}