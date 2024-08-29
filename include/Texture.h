#pragma once

#include "IncludesGl.h"
#include "Vertex.h"

#include <string>
#include <memory>
#include <map>

enum class TexMappingParam
{
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
};

enum class TextureDataTypes
{
    Float = GL_FLOAT,
    UByte = GL_UNSIGNED_BYTE, 
};

enum class TextureFormat
{
    RGBA = GL_RGBA,
    Red = GL_RED,
    RGBA16F = GL_RGBA16F,
};

enum class TexWrapParam
{
    Repeat = GL_REPEAT,
    ClampEdge = GL_CLAMP_TO_EDGE
};

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

class Texture
{
    using GLuint = unsigned int;

    enum class ImageFormat
    {
        Jpg,
        Png,
    };

public:
    void loadFromFile(std::string filename, TextureOptions options  = {});
    void create(int width, int height, TextureOptions options = {});

    void bind(int slot = 0);
    Vec2 getSize() const;
    GLuint getHandle() const;

private:
    void invalidate();
    void initialize(TextureOptions options);


private:
    GLuint m_texture_handle = 0;
    TextureOptions m_options;

    int m_width = 0;
    int m_height = 0;
};

struct TextureHolder
{

    bool add(std::string texture_name, Texture &texture)
    {
        if (m_textures.count(texture_name) != 0)
        {
            std::cout << "TEXTURE NAME EXISTS!\n"
                      << texture_name << "\n";
            return false;
        }

        m_textures[texture_name] = std::make_shared<Texture>(texture);

        return true;
    }

    bool remove(std::string name)
    {
    }

    bool add(std::string texture_name, std::string filename)
    {
        if (m_textures.count(texture_name) != 0)
        {
            std::cout << "TEXTURE NAME EXISTS!\n"
                      << texture_name << "\n";
            return false;
        }
        auto tex = std::make_shared<Texture>();
        tex->loadFromFile(filename);
        m_textures[texture_name] = std::move(tex);
        return true;
    }

    std::shared_ptr<Texture> get(std::string name)
    {
        if (m_textures.count(name) > 0)
            return m_textures.at(name);

        return nullptr;
    }

    std::map<std::string, std::shared_ptr<Texture>> m_textures;
};
