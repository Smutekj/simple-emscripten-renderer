#pragma once

#include "IncludesGl.h"
#include "Vertex.h"
#include "Shader.h"
#include "View.h"
#include "VertexArray.h"

#include <unordered_map>
#include <vector>
#include <memory>

constexpr static int BATCH_VERTEX_CAPACITY = 69000;

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

inline void hash_combine(std::size_t &seed) {}

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

// template <>
// struct std::hash<BatchConfig>
// {
//     std::size_t operator()(const Config &config) const
//     {
//         using std::hash;
//         using std::size_t;

//         // Compute individual hash values for first,
//         // second and third and combine them using XOR
//         // and bit shifting:
//         loop() return (std::hash<GLuint>()(config.shader_id) ^ (std::hash<GLuint>()(config.texture_id) << 1)) >> 1 ^ (std::hash<GLuint>()(config.draw_type) << 1);
//     }
// };

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
};

class SpriteBatch
{

public:
    SpriteBatch(BatchConfig config, Shader &shader)
        : m_config(config), m_shader(shader)
    {
        createBuffers();
    }

    SpriteBatch(GLuint texture_id, Shader &shader)
        : m_shader(shader), m_texture_id(texture_id)
    {
        createBuffers();
    }




    bool addSprite(Trans t)
    {
        if (m_end >= 2000)
        {
            return false;
        }
        m_transforms.at(m_end) = t;
        m_end++;
        return true;
    };

    void bindAttributes()
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

        glVertexAttribDivisor(1, 1);
        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glCheckError();
    }

    void initialize()
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

    int countFreeSpots() const
    {
        return m_transforms.size() - m_end;
    }

    void flush2(View &view)
    {
        m_shader.use();
        m_shader.setMat4("u_view_projection", view.getMatrix());
        m_shader.setUniforms();
        m_shader.activateTexture(0);
        
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

        m_end = 0;

        // glDisableVertexAttribArray(0);
        // glDisableVertexAttribArray(1);
        // glDisableVertexAttribArray(2);
        // glDisableVertexAttribArray(3);
        // glDisableVertexAttribArray(4);
        // glDisableVertexAttribArray(5);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    int getFreeVerts() const
    {
        return m_transforms.size() - m_end;
    }

    private:
        void createBuffers();

public:
    BatchConfig m_config;

private:
    static constexpr Vec2 m_prototype[6] = {{-1, -1},
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