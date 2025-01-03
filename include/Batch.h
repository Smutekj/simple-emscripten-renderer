#pragma once

#include "IncludesGl.h"
#include "Vertex.h"
#include "Shader.h"
#include "View.h"
#include "VertexArray.h"

#include <unordered_map>
#include <vector>
#include <memory>

constexpr static int BATCH_VERTEX_CAPACITY = 65000; //! maximum number of vertices per batch

//! \struct BatchConfig
//! \brief stores information which define batches
//! \brief each batch is defined by: 1. a set of GL texture ids 2. GL shader id and GL draw type
struct BatchConfig
{
    BatchConfig() = default;

    BatchConfig(TextureArray tex_ids, const GLuint &shader_id, DrawType draw_type);
    BatchConfig(const GLuint &tex_id, const GLuint &shader_id, DrawType draw_type);

    bool operator==(const BatchConfig &other) const;

    TextureArray texture_ids = {};
    GLuint shader_id = 0;
    DrawType draw_type = DrawType::Dynamic;
};

//! \class Batch
//! \brief used for batching draw calls of vertices with indices
//! \brief primitives are assumed to be triangles
class Batch
{

public:
    Batch(GLuint texture_id, Shader &shader, DrawType draw_type);
    Batch(const BatchConfig &config, Shader &shader, DrawType draw_type);
    ~Batch();

    void clear();

    void flush(View &view);

    void pushVertexArray(std::vector<Vertex> &verts);
    void pushVertex(Vertex v);
    void pushVertex(int ind);

    IndexType getLastInd() const
    {
        return m_used_vertices;
    }

    BatchConfig getConfig() const;

    int getFreeVerts() const;

protected:
    int m_used_vertices = 0;
    int m_capacity = BATCH_VERTEX_CAPACITY;

public:
    BatchConfig m_config;
    VertexArray m_verts;
    std::vector<IndexType> m_indices;

private:
    Shader& m_shader; 
};

//! \struct Trans
//! \brief contains data that gets into sprite shaders as attributes
struct Trans
{
    Vec2 trans = {0, 0};
    Vec2 scale = {1, 1};
    float angle = 0;
    Vec2 tex_coords = {0, 0};
    Vec2 tex_size = {0, 0};
    ColorByte color = {255, 255, 255, 255};
};

//! \class SpriteBatch
//! \brief manages batching of the sprite data
//! \brief can draw the batched sprites by using the flush method
class SpriteBatch
{

public:
    SpriteBatch(BatchConfig config, Shader &shader);
    SpriteBatch(GLuint texture_id, Shader &shader);
    ~SpriteBatch();

    bool addSprite(Trans t);
    void bindAttributes();
    void initialize();
    int countFreeSpots() const;
    int getFreeVerts() const;
    void flush(View &view);

private:
    void createBuffers();

public:
    BatchConfig m_config;

private:
    static constexpr Vec2 m_prototype[4] = {{-1, -1},
                                            {-1, 1},
                                            {1, -1},
                                            {1, 1}};

    static constexpr int m_indices[6] = {0, 1, 2, 3, 1, 2};

    Shader &m_shader;

    GLuint m_texture_id = 0;
    GLuint m_transform_buffer = 0; //<! buffer object with instancing data OpenGL id
    GLuint m_transform_vao = 0;    //<! vertex array object OpenGL id
    GLuint m_indices_buffer = 0;   //<! buffer object with element indices OpenGL id
    GLuint m_vbo = 0;              //<! vertex buffer object OpenGL id
    GLuint m_vao = 0;              //<! vertex array object for vertices OpenGL id

    std::array<Trans, BATCH_VERTEX_CAPACITY> m_transforms; //! transform and texture data is stored here
    int m_end = 0;
};

inline void hashloop(int n, std::invocable<int> auto &&hash_combiner)
{
    for (int i = 0; i < n; ++i)
    {
        std::invoke(hash_combiner, i);
    }
}

inline void hash_combine([[maybe_unused]] std::size_t &seed) {}

template <typename T, typename... Rest>
inline void hash_combine(std::size_t &seed, const T &v, Rest... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

template <>
struct std::hash<BatchConfig>
{
    std::size_t operator()(const BatchConfig &config) const
    {
        std::size_t ret = 0;
        hash_combine(ret, config.shader_id, config.draw_type, config.texture_ids[0], config.texture_ids[1]);
        return ret;
    }
};