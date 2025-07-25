#include "Texture.h"

#include "IncludesGl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stbimage/stb_image.h"

#include <cassert>
#include <iostream>


//! \brief constructs the texture from an \p image_file
//! \param image_file
//! \param options
Texture::Texture(std::filesystem::path image_file, TextureOptions options)
{
    loadFromFile(image_file.string(), options);   
}

//! \brief constructs an empty texture of a given dimensions
//! \param width
//! \param height
//! \param options
Texture::Texture(int width, int height, TextureOptions options)
{
    create(width, height, options);
}


Texture::~Texture()
{
    glDeleteTextures(1, &m_texture_handle);
    glCheckError();
}

//! \brief loads texture from file at \p filename
//! \param filename path to file
//! \param options  struct containing how the texture should be created
void Texture::loadFromFile(std::string filename, TextureOptions options)
{
    auto it = filename.find_last_of('.');
    auto format = filename.substr(it, filename.length());

    stbi_set_flip_vertically_on_load(true);
    //! load texture from file
    int channels_count;
    unsigned char *data = stbi_load(filename.c_str(), &m_width, &m_height, &channels_count, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (data)
    {
        //! generate name and bind texture
        initialize(options);
        glCheckError();
        auto format = channels_count == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);
        glCheckError();
        glGenerateMipmap(GL_TEXTURE_2D);
        glCheckError();

        stbi_image_free(data);
    }
    else
    {
        throw std::runtime_error("Error loading texture: " + filename);
    }
}

void Texture::invalidate()
{
    glDeleteTextures(1, &m_texture_handle);
    glCheckError();
}

//! \brief does the gl calls that set texture \p options
//! \param options  struct containing how the texture should be created
void Texture::initialize(TextureOptions options)
{

    glGenTextures(1, &m_texture_handle);
    glBindTexture(GL_TEXTURE_2D, m_texture_handle);
    glCheckError();

    // Set filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(options.wrap_x));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(options.wrap_y));
    glCheckError();

    assert(options.mag_param == TexMappingParam::Linear || options.mag_param == TexMappingParam::Nearest);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(options.min_param));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(options.mag_param));
    glCheckError();
}

//! \brief creates the texture with dimensions \p width x \p height and \p options
//! \param width
//! \param height
//! \param options  struct containing how the texture should be created
void Texture::create(int width, int height, TextureOptions options)
{
    m_options = options;
    m_width = width;
    m_height = height;
    initialize(options);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(options.internal_format),
                 width, height, 0,
                 static_cast<GLint>(options.format),
                 static_cast<GLint>(options.data_type),
                 NULL);
    glCheckError();
}

//! \brief bind the texture to a GL slot specified by: \p slot
//! \param slot
void Texture::bind(int slot)
{
    assert(m_texture_handle != 0); //! has to be generated first
    glActiveTexture(GL_TEXTURE0 + slot);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, m_texture_handle);
    glCheckError();
}

//! \brief bind the texture to a GL slot specified by: \p slot
//! \param slot
Vec2 Texture::getSize() const
{
    return {static_cast<float>(m_width), static_cast<float>(m_height)};
}

GLuint Texture::getHandle() const
{
    return m_texture_handle;
}

//! \brief adds texture into the holder under id \p texture_name
//! \param texture_name our id of the texture
//! \param texture
//! \returns true if no texture of this name exists othrewise return false;
bool TextureHolder::add(std::string texture_name, Texture &texture)
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

//! \brief reads the texture in \p texture_filename and adds it
//! \brief into the holder under id \p texture_name
//! \param texture_name our id of the texture
//! \param texture_filename     filename of the texture
//! \returns true if no texture of this name exists othrewise return false;
bool TextureHolder::add(std::string texture_name, std::string texture_filename)
{
    if (m_textures.count(texture_name) != 0)
    {
        std::cout << "TEXTURE NAME EXISTS!\n"
                  << texture_name << "\n";
        return false;
    }

    auto tex = std::make_shared<Texture>();
    tex->loadFromFile(m_resources_path.string() + texture_filename);
    m_textures[texture_name] = std::move(tex);
    return true;
}

std::shared_ptr<Texture> TextureHolder::get(std::string name) const
{
    if (m_textures.count(name) > 0)
        return m_textures.at(name);

    return nullptr;
}

std::map<std::string, std::shared_ptr<Texture>> &TextureHolder::getTextures()
{
    return m_textures;
}

//! \brief sets base path for searching shaders when loading
//! \param directory    path to a directory
//! \returns true if the \p directory is actually an existing directory, otherwise returns false
bool TextureHolder::setBaseDirectory(std::filesystem::path directory)
{
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        return false;
    }

    m_resources_path = directory;
    return true;
}

const TextureOptions &Texture::getOptions() const
{
    return m_options;
}