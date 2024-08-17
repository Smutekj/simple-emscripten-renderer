#include "Batch.h"

Batch::Batch(GLuint texture_id, Shader &shader, GLenum draw_type)
    : m_config(texture_id, shader.getId(), draw_type),
      m_verts(shader, draw_type, m_capacity)
{
}
Batch::Batch(const BatchConfig &config, Shader &shader, GLenum draw_type)
    : m_config(config),
      m_verts(shader, draw_type, m_capacity)
{
    std::for_each(config.texture_ids.begin(), config.texture_ids.end(), [&config, this](auto &id)
                  {
        int slot = &id - config.texture_ids.begin();
        m_verts.setTexture(slot, m_config.texture_ids.at(slot)); });
}

void Batch::clear()
{
    // m_verts.resize(0);
    m_used_vertices = 0;
    m_indices.clear();
}

void Batch::flush(View &view)
{
    // if(m_config.texture_id != 0)
    // {
    //     glBindTexture(GL_TEXTURE_2D, m_config.texture_ids.at);
    // }

    m_verts.draw(view, m_indices);

    if (m_config.draw_type != GL_STATIC_DRAW)
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
    // assert(ind >= 0 && ind < m_verts.size());
    m_indices.push_back(ind);
}

BatchConfig Batch::getConfig() const
{
    return m_config;
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