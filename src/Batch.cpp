#include "Batch.h"

#include "IncludesGl.h"


BatchI::~BatchI()
{
    glDeleteBuffers(1, &m_instance_buffer);
    glDeleteBuffers(1, &m_vertex_buffer);
    glDeleteVertexArrays(1, &m_vao);
}

std::shared_ptr<BatchI> makeSpriteBatch()
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
    vertex_data.insert(vertex_data.end(), (std::byte *)VERTEX_RECT, (std::byte *)(VERTEX_RECT) + sizeof(VERTEX_RECT));
    return std::make_unique<InstancedBatch>(vertex_data, layout);
}
std::shared_ptr<BatchI> makeVertexBatch()
{
    VAOId layout = makeVertexArrayVAO();
    return std::make_unique<VertexBatch>(layout);
}

std::shared_ptr<BatchI> makeTextBatch()
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
    return std::make_unique<InstancedBatch>(vertex_data, layout);
}

void InstancedBatch::flush(View &view, Shader &shader, TextureArray textures)
{
    if (m_instance_count == 0) //! no drawing of empty batches
    {
        return;
    }
    shader.setUniform("u_view_projection", view.getMatrix());
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

void VertexBatch::flush(View &view, Shader &shader, TextureArray textures)
{
    if (m_vertex_count == 0) //! no drawing of empty batches
    {
        return;
    }
    shader.setUniform("u_view_projection", view.getMatrix());
    shader.use();

    for (int tex_id = 0; tex_id < textures.size(); ++tex_id)
    {
        if (textures[tex_id] != 0)
        {
            glActiveTexture(GL_TEXTURE0 + tex_id); //! font texture;
            glBindTexture(GL_TEXTURE_2D, textures[tex_id]);
            glCheckError();
        }
    }

    //! send data to GPU and do the Draw Call
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_layout.vertices_size * m_vertex_count, m_vertex_data.data());
    glCheckError();
    glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);

    m_vertex_count = 0;
    m_vertex_data.clear();
    glBindVertexArray(0);
}

BatchI::BatchI(VAOId layout)
    : m_layout(layout)
{
}
void BatchI::addVertices(void *vertex_data, std::size_t data_size)
{
    m_vertex_count += data_size / m_layout.vertices_size;
    m_vertex_data.insert(m_vertex_data.end(), (std::byte *)vertex_data, (std::byte *)(vertex_data) + data_size);
}
void BatchI::addInstance(void *instance_data, std::size_t data_size)
{
    m_instance_count++;
    m_instance_data.insert(m_instance_data.end(), (std::byte *)instance_data, (std::byte *)(instance_data) + data_size);
}

GLuint BatchI::initVertexArrayObject(VAOId layout)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint vbo;
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, layout.vertices_size * layout.max_vertex_buffer_count, m_vertex_data.data(), GL_STATIC_DRAW);

    std::size_t offset = 0;
    std::size_t attrib_id = 0;
    for (auto attrib : layout.vertex_attirbutes)
    {
        glEnableVertexAttribArray(attrib_id);

        glVertexAttribPointer(attrib_id, attrib.count, attrib.type_id, attrib.is_normalized,
                              layout.vertices_size, (void *)(offset));
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
    glBufferData(GL_ARRAY_BUFFER, layout.instance_size * m_layout.max_instance_count, m_instance_data.data(), GL_STREAM_DRAW);

    for (auto attrib : layout.instanced_attributes)
    {
        glEnableVertexAttribArray(attrib_id);

        if (attrib.type_id == GL_INT)
        {
            glVertexAttribIPointer(attrib_id, attrib.count, attrib.type_id,
                                   layout.instance_size, (void *)(offset));
        }
        else
        {
            glVertexAttribPointer(attrib_id, attrib.count, attrib.type_id, attrib.is_normalized,
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
    glGenBuffers(1, &m_vertex_buffer);
    initVertexArrayObject(layout);
}

void VertexBatch::addVertices(void *data, std::size_t data_size)
{
    BatchI::addVertices(data, data_size);
}

InstancedBatch::InstancedBatch(std::vector<std::byte> vertex_data, VAOId layout)
    : BatchI(layout)
{
    glGenBuffers(1, &m_vertex_buffer);
    glGenBuffers(1, &m_instance_buffer);
    addVertices(vertex_data.data(), vertex_data.size());
    initVertexArrayObject(layout);
}

// #define PARENS ()

// /*---------------------------------------------------------------------------------*/
// #define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
// #define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
// #define EXPAND3(...) __VA_ARGS__
// /*---------------------------------------------------------------------------------*/
// #define FOR_EACH(macro, ...) \
//     __VA_OPT__(EXPAND(FOR_EACH_HELPER(macro, __VA_ARGS__)))
// #define FOR_EACH_HELPER(macro, a1, ...) \
//     macro(a1)                           \
//         __VA_OPT__(, FOR_EACH_AGAIN PARENS(macro, __VA_ARGS__))
// #define FOR_EACH_AGAIN() FOR_EACH_HELPER
// /*---------------------------------------------------------------------------------*/
// #define FOR_EACH_P(macro, first, ...) \
//     __VA_OPT__(EXPAND(FOR_EACH_HELPER_P(macro, first, __VA_ARGS__)))
// #define FOR_EACH_HELPER_P(macro, first, a1, ...) \
//     macro(first, a1)                             \
//         __VA_OPT__(, FOR_EACH_AGAIN_P PARENS(macro, first, __VA_ARGS__))
// #define FOR_EACH_AGAIN_P() FOR_EACH_HELPER_P
// /*---------------------------------------------------------------------------------*/
// #define STRINGIFY(arg) #arg
// #define NUMARGS(...) (sizeof((const char *[]){FOR_EACH(STRINGIFY, __VA_ARGS__)}) / sizeof(const char *))
// /*---------------------------------------------------------------------------------*/
// #define SIZEOF_MEMBER(Base, member) \
//     sizeof(Base::member)
// /*---------------------------------------------------------------------------------*/
// #define DESCRIBE_ATTRIBUTE(AttributeType, ...)                                                              \
//     namespace describe                                                                                      \
//     {                                                                                                       \
//         class AttributeType##_DESCRIPTOR                                                                    \
//         {                                                                                                   \
//         public:                                                                                             \
//             inline static int ATTRIB_COUNT = NUMARGS(__VA_ARGS__);                                          \
//             inline static std::vector<int> OFFSETS = {FOR_EACH_P(offsetof, AttributeType, __VA_ARGS__)};    \
//             inline static std::vector<int> SIZES = {FOR_EACH_P(SIZEOF_MEMBER, AttributeType, __VA_ARGS__)}; \
//         };                                                                                                  \
//                                                                                                             \
//         template <>                                                                                         \
//         std::vector<int> getOffsets<AttributeType>()                                                        \
//         {                                                                                                   \
//             return AttributeType##_DESCRIPTOR::OFFSETS;                                                     \
//         }                                                                                                   \
//         template <>                                                                                         \
//         std::vector<int> getSizes<AttributeType>()                                                          \
//         {                                                                                                   \
//             return AttributeType##_DESCRIPTOR::SIZES;                                                       \
//         }                                                                                                   \
//     }

// /*---------------------------------------------------------------------------------*/

// namespace describe
// {
//     template <class DescribedType>
//     std::vector<int> getOffsets() { return {}; }
//     template <class DescribedType>
//     std::vector<int> getSizes() { return {}; }
// };

// struct Instance
// {
//     char x;
//     double y;
// };
// DESCRIBE_ATTRIBUTE(Instance, x, y);
// DESCRIBE_ATTRIBUTE(Vertex, pos, color, tex_coord);

// template <class VertexData, class InstanceData>
// VAOId getVAOId()
// {
//     VAOId id;
//     // id.instance_size = si
//     auto offsets = describe::getOffsets<InstanceData>();
//     id.instanced_attributes;

//     return id;
// }