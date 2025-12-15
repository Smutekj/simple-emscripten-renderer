#pragma once

#include "GLTypeDefs.h"
#include <vector>

class Shader;

//! \struct BatchConfig
//! \brief stores information which define batches
//! \brief each batch is defined by: 1. a set of GL texture ids 2. GL shader id and GL draw type
struct BatchConfig
{
    BatchConfig() = default;

    BatchConfig(TextureArray tex_ids, const GLuint &shader_id, DrawType draw_type = DrawType::Dynamic);
    BatchConfig(const GLuint &tex_id, const GLuint &shader_id, DrawType draw_type = DrawType::Dynamic);
    BatchConfig(TextureArray tex_ids, Shader* shader_id, DrawType draw_type = DrawType::Dynamic);

    bool operator==(const BatchConfig &other) const;

    TextureArray texture_ids = {};
    GLuint shader_id = 0;
    DrawType draw_type = DrawType::Dynamic;

    Shader* p_shader = nullptr;
};


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
