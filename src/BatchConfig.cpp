#include "BatchConfig.h"
#include "Shader.h"

//! \brief constructs a batch from an array of \p texture_ids a shader_id and a \p draw_type
//! \param tex_ids      an array of  GL texture ids
//! \param shader_id    GL shader id
//! \param draw_type    Dynamic or Static draws
BatchConfig::BatchConfig(TextureArray tex_ids, const GLuint &shader_id, DrawType draw_type)
    : shader_id(shader_id), draw_type(draw_type)
{
    std::copy(tex_ids.begin(), tex_ids.end(), texture_ids.begin());
}
BatchConfig::BatchConfig(TextureArray tex_ids, Shader *p_shader, DrawType draw_type)
    : shader_id(p_shader->getId()), p_shader(p_shader), draw_type(draw_type)
{
    std::copy(tex_ids.begin(), tex_ids.end(), texture_ids.begin());
}

//! \brief constructs a batch from a single texture_id a shader_id and a \p draw_type
//! \brief all other texture_ids in the texture_id array are assumed to be 0;
//! \param tex_id   GL texture id
//! \param shader_id    GL shader id
//! \param draw_type    Dynamic or Static draws
BatchConfig::BatchConfig(const GLuint &tex_id, const GLuint &shader_id, DrawType draw_type)
    : shader_id(shader_id), draw_type(draw_type)
{
    texture_ids[0] = tex_id;
}

//! \brief are two configurations the same?
//! \brief needed by the hash table
bool BatchConfig::operator==(const BatchConfig &other) const
{
    bool shaders_same = other.shader_id == shader_id;
    bool textures_same = std::equal(texture_ids.begin(), texture_ids.end(), std::begin(other.texture_ids));
    bool drawtypes_same = draw_type == other.draw_type;
    return shaders_same && textures_same && drawtypes_same;
}