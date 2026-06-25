#include "Batch.h"

#include <queue>

#include "Logging.h"
#include "Shader.h"
#include "ViewMatrix.h"
#include "IncludesGl.h"

BatchI::BatchI(VAOId layout)
    : m_layout(layout)
{
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_instance_buffer);
    glGenVertexArrays(1, &m_vao);
}

BatchI::~BatchI()
{
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteBuffers(1, &m_instance_buffer);
    glDeleteVertexArrays(1, &m_vao);
}

std::unique_ptr<BatchI> makeSpriteBatch()
{
    static constexpr float VERTEX_RECT[6 * 4] = {
        -1, -1, 0, 0,
        -1, +1, 0, 1,
        +1, -1, 1, 0,
        +1, +1, 1, 1,
        +1, -1, 1, 0,
        -1, +1, 0, 1};

    VAOId layout = makeSpriteVAO();

    std::vector<std::byte> vertex_data;
    vertex_data.insert(vertex_data.end(),
                       (std::byte *)VERTEX_RECT,
                       (std::byte *)(VERTEX_RECT) + sizeof(VERTEX_RECT));

    auto p_batch = std::make_unique<DepthBatch<SpriteInstance>>(vertex_data, layout);

    p_batch->m_depth_offset = offsetof(SpriteInstance, depth);
    p_batch->m_instance_size = layout.instance_size;

    return std::move(p_batch);
}

std::unique_ptr<BatchI> makeVertexBatch()
{
    VAOId layout = makeVertexArrayVAO();
    auto p_batch = std::make_unique<VertexBatch>(layout);
    p_batch->m_instance_size = sizeof(Vertex);
    p_batch->m_depth_offset = offsetof(Vertex, depth);
    return std::move(p_batch);
}

std::unique_ptr<BatchI> makeTextBatch()
{
    static constexpr float VERTEX_RECT[6 * 4] = {
        -1, -1, 0, 0,
        -1, +1, 0, 1,
        +1, -1, 1, 0,
        +1, +1, 1, 1,
        +1, -1, 1, 0,
        -1, +1, 0, 1};

    VAOId layout = makeTextVAO();
    std::vector<std::byte> vertex_data;
    vertex_data.insert(vertex_data.end(), (std::byte *)VERTEX_RECT, (std::byte *)(VERTEX_RECT) + sizeof(VERTEX_RECT));

    auto p_batch = std::make_unique<DepthBatch<TextInstance>>(vertex_data, layout);

    p_batch->m_depth_offset = offsetof(SpriteInstance, depth);
    p_batch->m_instance_size = layout.instance_size;

    return p_batch;
}

void InstancedBatch::flush(std::size_t begin, std::size_t end, RenderContext context)
{
    assert(end <= m_instance_count && end >= begin);
    if (m_instance_count == 0 || begin == end) //! no drawing of empty batches
    {
        return;
    }

    context.p_shader->setUniform("u_view_projection", getMatrix(context.view));
    context.p_shader->use();
    glCheckError();

    for (std::size_t i = 0; i < context.tex_ids.size(); ++i)
    {
        if (context.tex_ids[i] != 0)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, context.tex_ids[i]);
            glCheckError();
        }
    }

    //! send data to GPU
    std::size_t instance_count = end - begin;
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
    glCheckError();
//! the actual draw call
#if defined(GLES)
    glBufferSubData(GL_ARRAY_BUFFER,
            0,
            m_layout.instance_size * instance_count,
            m_instance_data.data() + m_layout.instance_size * begin);
    glCheckError();
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertex_count, instance_count);
#else
    glBufferSubData(GL_ARRAY_BUFFER,
                    m_layout.instance_size * begin,
                    m_layout.instance_size * instance_count,
                    m_instance_data.data() + m_layout.instance_size * begin);
    glCheckError();
    glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, m_vertex_count, instance_count, begin);
#endif
    glCheckError();
    //! reset instance count (Should we add option to also reset vertex count?)
    glBindVertexArray(0);

    if (end == m_instance_count)
    {
        m_instance_count = 0;
        m_instance_data.clear();
    }
}

void InstancedBatch::flush(std::size_t begin, RenderContext context)
{
    flush(begin, m_instance_count, context);
}

void InstancedBatch::flush(View &view, Shader &shader, TextureArray textures)
{
    if (m_instance_count == 0) //! no drawing of empty batches
    {
        return;
    }
    shader.setUniform("u_view_projection", getMatrix(view));
    shader.use();
    glCheckError();

    for (int tex_id = 0; tex_id < textures.size(); ++tex_id)
    {
        if (textures[tex_id] != 0)
        {
            glActiveTexture(GL_TEXTURE0 + tex_id);
            glBindTexture(GL_TEXTURE_2D, textures[tex_id]);
            glCheckError();
        }
    }

    //! send data to GPU
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
    glCheckError();
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_layout.instance_size * m_instance_count, m_instance_data.data());
    glCheckError();
    //! the actual draw call
    glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertex_count, m_instance_count);
    glCheckError();
    //! reset instance count (Should we add option to also reset vertex count?)
    m_instance_count = 0;
    m_instance_data.clear();

    glBindVertexArray(0);
}

void VertexBatch::flush(std::size_t begin, std::size_t end, RenderContext context)
{
    assert(begin % 3 == 0);
    assert(end % 3 == 0); // draw by triangles
    assert(begin <= end && end <= m_vertex_count);
    if (m_vertex_count == 0 || begin == end) //! no drawing of empty batches
    {
        return;
    }
    context.p_shader->setUniform("u_view_projection", getMatrix(context.view));
    context.p_shader->use();

    for (std::size_t i = 0; i < context.tex_ids.size(); ++i)
    {
        if (context.tex_ids[i] != 0)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, context.tex_ids[i]);
            glCheckError();
        }
    }

    //! send data to GPU and do the Draw Call
    std::size_t vertex_count = end - begin;
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER,
                    m_layout.vertex_size * begin,
                    m_layout.vertex_size * vertex_count,
                    m_vertex_data.data() + begin * m_layout.vertex_size);
    glCheckError();
    glDrawArrays(GL_TRIANGLES, begin, vertex_count);

    if (end == m_vertex_count)
    {
        m_vertex_count = 0;
        m_instance_count = 0;
        m_vertex_data.clear();
    }

    glBindVertexArray(0);
}
void VertexBatch::flush(std::size_t begin, RenderContext context)
{
    flush(begin, m_vertex_count, context);
}
void VertexBatch::flush(View &view, Shader &shader, TextureArray textures)
{
    if (m_vertex_count == 0) //! no drawing of empty batches
    {
        return;
    }
    shader.setUniform("u_view_projection", getMatrix(view));
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

    //! send data to GPU and do the Draw Call
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    m_layout.vertex_size * m_vertex_count,
                    m_vertex_data.data());
    glCheckError();
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);

    m_vertex_count = 0;
    m_vertex_data.clear();
    glBindVertexArray(0);
}

void BatchI::addVertices(const void *vertex_data, std::size_t data_size)
{
    m_vertex_count += data_size / m_layout.vertex_size;
    m_instance_count = m_vertex_count;
    m_vertex_data.insert(m_vertex_data.end(), (std::byte *)vertex_data, (std::byte *)(vertex_data) + data_size);
}
void BatchI::addInstance(const void *instance_data, std::size_t data_size)
{
    m_instance_count++;
    m_instance_data.insert(m_instance_data.end(), (std::byte *)instance_data, (std::byte *)(instance_data) + data_size);
}

GLuint BatchI::initVertexArrayObject(VAOId layout)
{
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 layout.vertex_size * layout.max_vertex_buffer_count,
                 m_vertex_data.data(),
                 GL_STATIC_DRAW);

    std::size_t offset = 0;
    std::size_t attrib_id = 0;
    for (auto attrib : layout.vertex_attirbutes)
    {
        glEnableVertexAttribArray(attrib_id);

        glVertexAttribPointer(attrib_id,
                              attrib.count,
                              attrib.type_id,
                              attrib.is_normalized,
                              layout.vertex_size, (void *)(offset));
        glCheckError();

        glVertexAttribDivisor(attrib_id, 0);
        attrib_id++;
        offset += attrib.size;
    }
    offset = 0;

    //! instance buffer is created only when we use instanced rendering
    if (layout.instanced_attributes.empty())
    {
        glBindVertexArray(0);
        return m_vao;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 layout.instance_size * m_layout.max_instance_count,
                 m_instance_data.data(),
                 GL_STREAM_DRAW);

    for (auto attrib : layout.instanced_attributes)
    {
        glEnableVertexAttribArray(attrib_id);
        glCheckError();

        if (attrib.type_id == GL_INT)
        {
            glVertexAttribIPointer(attrib_id,
                                   attrib.count,
                                   attrib.type_id,
                                   layout.instance_size,
                                   (void *)(offset));
        }
        else
        {
            glVertexAttribPointer(attrib_id,
                                  attrib.count,
                                  attrib.type_id,
                                  attrib.is_normalized,
                                  layout.instance_size, (void *)(offset));
        }
        glCheckError();
        glVertexAttribDivisor(attrib_id, 1);
        glCheckError();
        attrib_id++;
        offset += attrib.size;
    }
    glCheckError();
    glBindVertexArray(0);

    return m_vao;
}
VertexBatch::VertexBatch(VAOId layout)
    : BatchI(layout)
{
    initVertexArrayObject(layout);
}

void VertexBatch::sortByDepth()
{
    m_buffer_with_depth = m_vertex_data.data();
    auto *p_data = reinterpret_cast<std::array<Vertex, 3> *>(m_vertex_data.data());

    std::sort(p_data,
              p_data + m_vertex_count / 3,
              [](auto &i1, auto &i2)
              { return i1[0].depth < i2[0].depth; });
}

InstancedBatch::InstancedBatch(std::vector<std::byte> vertex_data, VAOId layout)
    : BatchI(layout)
{
    addVertices(vertex_data.data(), vertex_data.size());
    m_instance_count = 0;
    initVertexArrayObject(layout);
}

void BatchRegistry::renderAll(View &view)
{
    // for (auto &batch_holder : m_batches)
    // {
    //     for (auto &[config, batch] : batch_holder)
    //     {
    //         batch->flush(view, *config.p_shader, config.texture_ids);
    //     }
    // }

    struct BatchOrder
    {
        float lowest;
        std::size_t lowest_id;
        std::size_t batch_id;
    };
    std::vector<BatchOrder> p_batches;

    struct RenderData
    {
        BatchI *p_batch;
        RenderContext context;
    };
    std::vector<RenderData> batches;

    int batch_id = 0;
    for (auto &batch_holder : m_batches)
    {
        for (auto &[config, batch] : batch_holder)
        {
            if (batch->m_instance_count == 0)
            {
                continue;
            }
            batch->sortByDepth();
            batches.emplace_back(batch.get(),
                                 RenderContext{.view = view,
                                               .p_shader = config.p_shader,
                                               .tex_ids = config.texture_ids});
            p_batches.emplace_back(batch->getDepth(0), 0, batch_id);
            batch_id++;
        }
    }

    std::priority_queue pq(begin(p_batches), end(p_batches), [](auto &b1, auto &b2)
                           { return b1.lowest > b2.lowest; });

    if (pq.empty())
    {
        return;
    }

    auto [lowest, lowest_id, current_batch_id] = pq.top();
    pq.pop();

    int flush_count = 0;
    while (!pq.empty())
    {
        auto [second_lowest, next_lowest_id, next_batch_id] = pq.top();
        pq.pop();
        auto &[current_batch, context] = batches.at(current_batch_id);

        std::size_t flush_end = current_batch->firstHigherThan(lowest_id, second_lowest);

        flush_count++;
        current_batch->flush(lowest_id, flush_end, context);

        if (current_batch->m_instance_count > 0)
        {
            //! it is zero when all has been flushed
            pq.push({current_batch->getDepth(flush_end),
                     flush_end,
                     current_batch_id});
        }
        current_batch_id = next_batch_id;
        lowest = second_lowest;
        lowest_id = next_lowest_id;
    }

    flush_count++;
    auto &[current_batch, context] = batches.at(current_batch_id);
    current_batch->flush(lowest_id,
                         context);
}
