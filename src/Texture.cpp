#include "Texture.h"

#include "IncludesGl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stbimage/stb_image.h"
#include <cassert>
#include <iostream>

void Texture::loadFromFile(std::string filename)
{
    auto it = filename.find_last_of('.');
    auto format = filename.substr(it, filename.length());

    // stbi_set_flip_vertically_on_load(false);
    //! load texture from file
    int channels_count;
    unsigned char *data = stbi_load(filename.c_str(), &m_width, &m_height, &channels_count, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (data)
    {
        //! generate name and bind texture
        initialize();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
void Texture::initialize()
{

    glGenTextures(1, &m_texture_handle);
    glBindTexture(GL_TEXTURE_2D, m_texture_handle);
    glCheckError();

    // Set filtering
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glCheckError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glCheckError();
}

void Texture::create(int width, int height, GLint internal_format, GLint format, GLint channel_format)
{
    m_width = width;
    m_height = height;
    initialize();
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, channel_format, NULL);
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