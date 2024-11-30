#pragma once

#include "IncludesGl.h"
#include "Vertex.h"

#include <filesystem>
#include <string>
#include <memory>
#include <map>

using TextureHandle = GLuint;

//! \enum TexMappingParam
//! \brief options for texture mapping procedure
//! translates into corresponding OpenGL integers explanations are here:
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TexMappingParam
{
    Nearest = GL_NEAREST,                               
    Linear = GL_LINEAR,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
};

//! \enum TextureDataTypes
//! \brief options for data types stored in texture pixels
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TextureDataTypes
{
    Float = GL_FLOAT,
    UByte = GL_UNSIGNED_BYTE,
};

//! \enum TextureFormat
//! \brief format of the data stored in each texture pixel 
//! this will influences the data format of the texture pixels in shaders
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TextureFormat
{
    RGBA = GL_RGBA,
    Red = GL_RED,
    RGBA16F = GL_RGBA16F,
};

//! \enum TexWrapParam
//! \brief specifies the boundary condition used in drawing textures
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
enum class TexWrapParam
{
    Repeat = GL_REPEAT,         //!< uses periodic boundary conditions in texture space (cool for making patters)
    ClampEdge = GL_CLAMP_TO_EDGE //!< uses fixed value boundary condition in texture space, if the 
};

//! \struct TextureOptions
//! \brief aggregates different OpenGL texture configurations  
//! based exactly on these options:
//! \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glTexParameter.xhtml
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

//! \class Texture
//! \brief manages data in the texture
//!  This class corresponds to the actual data on the GPU
//!  Textures should be created sparingly, whereas we can have as many Sprites as we want 
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

    const TextureOptions& getOptions()const;

private:
    void invalidate();
    void initialize(TextureOptions options);

private:
    GLuint m_texture_handle = 0;        //! OpenGL id for texture
    TextureOptions m_options;

    int m_width = 0;
    int m_height = 0;
};
 
 //! \class TextureHolder
 //! \brief holds textures based on id given by string
class TextureHolder
{

public:
    bool add(std::string texture_name, Texture &texture);
    bool add(std::string texture_name, std::string filename);

    std::shared_ptr<Texture> get(std::string name);
    std::map<std::string, std::shared_ptr<Texture>>& getTextures();

    bool setBaseDirectory(std::filesystem::path directory);

private:
    std::map<std::string, std::shared_ptr<Texture>> m_textures;
    std::filesystem::path m_resources_path;
};
