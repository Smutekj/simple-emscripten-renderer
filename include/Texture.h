#pragma once

#include "IncludesGl.h"
#include "Vertex.h"

#include <string>
#include <memory>
#include <map>

using TextureHandle = GLuint;

constexpr int N_MAX_TEXTURES = 2; //! maximum number of texture slots

//! \enum options for texture mapping procedure
//! \brief translates into corresponding OpenGL integers
enum class TexMappingParam
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
};

//! \enum options for data types stored in texture pixels
enum class TextureDataTypes
{
    Float = GL_FLOAT,
    UByte = GL_UNSIGNED_BYTE,
};

//! \enum format of the data stored in each texture pixel 
//! \brief this needs to correspond somehow to how shaders are used 
enum class TextureFormat
{
    RGBA = GL_RGBA,
    Red = GL_RED,
    RGBA16F = GL_RGBA16F,
};

//! \enum specifies the boundary condition used in drawing textures
enum class TexWrapParam
{
    Repeat = GL_REPEAT,
    ClampEdge = GL_CLAMP_TO_EDGE
};

//! \struct aggregates different OpenGL texture configurations  
struct TextureOptions
{
    TextureFormat format = TextureFormat::RGBA;
    TextureFormat internal_format = TextureFormat::RGBA16F;
    TextureDataTypes data_type = TextureDataTypes::Float;
    TexMappingParam mag_param = TexMappingParam::Linear;
    TexMappingParam min_param = TexMappingParam::Linear;
    TexWrapParam wrap_x = TexWrapParam::Repeat;
    TexWrapParam wrap_y = TexWrapParam::Repeat;
};

//! \class manages data in the texture
//! \brief This class corresponds to the actual data on the GPU
//! \brief Textures should be created sparingly, whereas we can have as many Sprites as we want 
class Texture
{
    using GLuint = unsigned int;

    enum class ImageFormat
    {
        Jpg,
        Png,
    };

public:
    void loadFromFile(std::string filename, TextureOptions options = {});
    void create(int width, int height, TextureOptions options = {});

    void bind(int slot = 0);
    Vec2 getSize() const;
    GLuint getHandle() const;

private:
    void invalidate();
    void initialize(TextureOptions options);

private:
    GLuint m_texture_handle = 0;        //! OpenGL id for texture
    TextureOptions m_options;

    int m_width = 0;
    int m_height = 0;
};
 
 //! \class holds textures based on id given by string
class TextureHolder
{

public:
    bool add(std::string texture_name, Texture &texture);
    bool add(std::string texture_name, std::string filename);

    std::shared_ptr<Texture> get(std::string name);

private:
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
};
