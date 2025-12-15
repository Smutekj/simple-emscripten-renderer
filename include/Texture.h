#pragma once

#include "GLTypeDefs.h"
#include "Vertex.h"

#include <filesystem>
#include <string>
#include <memory>
#include <unordered_map>

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
    TexMappingParam min_param = TexMappingParam::LinearMipmapLinear;
    TexWrapParam wrap_x = TexWrapParam::ClampEdge;
    TexWrapParam wrap_y = TexWrapParam::ClampEdge;
};

//! \class Texture
//! \brief manages data in the texture
//!  This class corresponds to the actual data on the GPU
//!  Textures should be created sparingly, whereas we can have as many Sprites as we want
class Texture
{
    enum class ImageFormat
    {
        Jpg,
        Png,
    };

public:
    Texture() = default;
    Texture(std::filesystem::path image_file, TextureOptions options = {});
    Texture(int width, int height, TextureOptions options = {});
    Texture(const unsigned char *buffer, std::size_t size, TextureOptions options = {});

    ~Texture();
    Texture(const Texture &other);
    Texture(Texture &&other) = default;
    Texture &operator=(const Texture &other) = default;
    Texture &operator=(Texture &&other) = default;

    void loadFromFile(std::string filename, TextureOptions options = {});
    void loadFromBytes(const unsigned char *buffer, std::size_t size, TextureOptions options = {});
    void create(int width, int height, TextureOptions options = {});

    void setWrapX(TexWrapParam wrap_x);
    void setWrapY(TexWrapParam wrap_y);
    void setMappingMinify(TexMappingParam map_min);
    void setMappingMagnify(TexMappingParam map_mag);

    void bind(int slot = 0);
    Vec2 getSize() const;
    TextureHandle getHandle() const;

    const TextureOptions &getOptions() const;

private:
    void invalidate();
    void initialize(TextureOptions options);

private:
    GLuint m_texture_handle = 0; //! OpenGL id for texture
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
    bool add(std::string texture_name, std::string filename, TextureOptions opt = {});
    bool add(std::string texture_name, std::filesystem::path texture_file_path, TextureOptions opt = {});
    bool add(std::string texture_name, const unsigned char *buffer, std::size_t size, TextureOptions opt = {});

    void erase(const std::string &texture_id);

    std::shared_ptr<Texture> get(std::string name) const;
    std::unordered_map<std::string, std::shared_ptr<Texture>> &getTextures();

    bool setBaseDirectory(std::filesystem::path directory);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
    std::filesystem::path m_resources_path;
};
