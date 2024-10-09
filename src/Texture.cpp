#include "Texture.h"

#include "IncludesGl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stbimage/stb_image.h"

#include <cassert>
#include <iostream>

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
        throw std::runtime_error("Error loading texture.");
    }
}

void Texture::invalidate()
{
    glDeleteTextures(1, &m_texture_handle);
    glCheckError();
}
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

void Texture::bind(int slot)
{
    // assert(m_texture_handle != 0); //! has to be generated first
    glActiveTexture(GL_TEXTURE0 + slot);
    glCheckError();
    glBindTexture(GL_TEXTURE_2D, m_texture_handle);
    glCheckError();
}

Vec2 Texture::getSize() const
{
    return {static_cast<float>(m_width), static_cast<float>(m_height)};
}

GLuint Texture::getHandle() const
{
    return m_texture_handle;
}

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

bool TextureHolder::add(std::string texture_name, std::string filename)
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

std::shared_ptr<Texture> TextureHolder::get(std::string name)
{
    if (m_textures.count(name) > 0)
        return m_textures.at(name);

    return nullptr;
}
