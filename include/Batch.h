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

struct BatchConfig
{

    BatchConfig() = default;

    BatchConfig(std::array<GLuint, N_MAX_TEXTURES> tex_ids, const GLuint &shader_id, GLenum draw_type)
        : shader_id(shader_id), draw_type(draw_type)
    {
        assert(tex_ids.size() <= N_MAX_TEXTURES);
        std::copy(tex_ids.begin(), tex_ids.end(), texture_ids.begin());
    }
    BatchConfig(const GLuint &tex_id, const GLuint &shader_id, GLenum draw_type)
        : shader_id(shader_id), draw_type(draw_type)
    {
        texture_ids[0] = tex_id;
    }

    bool operator==(const BatchConfig &other) const
    {
        bool shaders_same = other.shader_id == shader_id;
        bool textures_same = std::equal(texture_ids.begin(), texture_ids.end(), std::begin(other.texture_ids));
        bool drawtypes_same = draw_type == other.draw_type;
        return shaders_same && textures_same && drawtypes_same;
    }

    std::array<GLuint, N_MAX_TEXTURES> texture_ids = {0, 0};
    // GLuint texture_id = 0;
    GLuint shader_id = 0;
    GLenum draw_type = GL_DYNAMIC_DRAW;
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
struct
    std::hash<BatchConfig>
{
    std::size_t operator()(const BatchConfig &config) const
    {
        std::size_t ret = 0;
        hash_combine(ret, config.shader_id, config.draw_type, config.texture_ids[0], config.texture_ids[1]);
        return ret;
    }
};

class Batch
{

public:
    Batch(GLuint texture_id, Shader &shader, GLenum draw_type);
    Batch(const BatchConfig &config, Shader &shader, GLenum draw_type);

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
};

struct Trans
{
    Vec2 trans = {0, 0};
    Vec2 scale = {1, 1};
    float angle = 0;
    Vec2 tex_coords = {0, 0};
    Vec2 tex_size = {0, 0};
    ColorByte color = {255, 255, 255, 255};
};

class SpriteBatch
{

public:
    SpriteBatch(BatchConfig config, Shader &shader);
    SpriteBatch(GLuint texture_id, Shader &shader);

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
    GLuint m_transform_buffer = 0;
    GLuint m_indices_buffer = 0;
    GLuint m_vbo = 0;

    std::array<Trans, BATCH_VERTEX_CAPACITY> m_transforms;
    int m_end = 0;
};