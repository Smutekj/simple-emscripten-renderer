#pragma once

#include "GLTypeDefs.h"
#include "Vertex.h"
#include "glad/glad.h"
#include "Rect.h"

#include <vector>

//! \struct SpriteInstance 
//! \brief contains data that gets into sprite shaders as attributes
struct SpriteInstance
{
    Vec2 trans = {0, 0};
    Vec2 scale = {1, 1};
    float angle = 0;
    Vec2 tex_coords = {0, 0};
    Vec2 tex_size = {0, 0};
    ColorByte color = {255, 255, 255, 255};
};
//! \struct TextInstance
//! \brief data that get sent into text shaders
struct TextInstance
{
    utils::Vector2f pos = {0, 0};
    utils::Vector2f scale = {1, 1};
    float angle = 0.f;
    ColorByte edge_color = {0, 0, 0, 0};
    ColorByte fill_color = {1, 1, 1, 1};
    ColorByte glow_color = {0, 0, 0, 0};
    int char_code;
    float start_time = 0.f;
};

//! \struct SpriteInstance 
//! \brief contains data that gets into sprite shaders as attributes
struct BlurredRectInstance
{
    Vec2 trans = {0, 0};
    Vec2 scale = {1, 1};
    float angle = 0;
    float blur_radius = 1.f;
    float border_width = 0.f;
    ColorByte color = {255, 255, 255, 255};
    ColorByte border_color = {255, 255, 255, 255};
};

struct AttributeId
{
    GLuint type_id;
    std::size_t count; //!< number of components 
    std::size_t size;  //!< size on CPU in bytes
    bool is_normalized = false;

    bool operator==(const AttributeId &other) const noexcept
    {
        return type_id == other.type_id &&
               size == other.size;
    }
};

struct VAOId
{
    using AttributeList = std::vector<AttributeId>;

    AttributeList instanced_attributes;
    AttributeList vertex_attirbutes;

    std::size_t instance_size;
    std::size_t vertices_size;

    std::size_t max_vertex_buffer_count;
    std::size_t max_instance_count;

    bool operator==(const VAOId &other) const noexcept
    {
        return instanced_attributes == other.instanced_attributes &&
               vertex_attirbutes == other.vertex_attirbutes;
    }
};

template <class T>
inline AttributeId makeAttribute()
{
    if constexpr (std::is_same_v<T, utils::Vector2f>)
    {
        return {.type_id = GL_FLOAT, .count = 2, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        return {.type_id = GL_INT, .count = 1, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, ColorByte>)
    {
        return {.type_id = GL_UNSIGNED_BYTE, .count = 4, .size = sizeof(T), .is_normalized = true};
    }
    else if constexpr (std::is_same_v<T, Color>)
    {
        return {.type_id = GL_FLOAT, .count = 4, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return {.type_id = GL_FLOAT, .count = 1, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, Rectf>)
    {
        return {.type_id = GL_FLOAT, .count = 4, .size = sizeof(T), .is_normalized = false};
    }
    return {};
};

template <class T>
inline AttributeId makeAttribute(T var)
{
    if constexpr (std::is_same_v<T, utils::Vector2f>)
    {
        return {.type_id = GL_FLOAT, .count = 2, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, int>)
    {
        return {.type_id = GL_INT, .count = 1, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, ColorByte>)
    {
        return {.type_id = GL_UNSIGNED_BYTE, .count = 4, .size = sizeof(T), .is_normalized = true};
    }
    else if constexpr (std::is_same_v<T, Color>)
    {
        return {.type_id = GL_FLOAT, .count = 4, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return {.type_id = GL_FLOAT, .count = 1, .size = sizeof(T), .is_normalized = false};
    }
    else if constexpr (std::is_same_v<T, Rectf>)
    {
        return {.type_id = GL_FLOAT, .count = 4, .size = sizeof(T), .is_normalized = false};
    }
    return {};
};

VAOId makeVertexArrayVAO();
VAOId makeSpriteVAO();
VAOId makeTextVAO();


