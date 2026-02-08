#include "Texture.h"

#include "IncludesGl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../external/stbimage/stb_image.h"

#include <cassert>

//! \brief constructs the texture from an \p image_file
//! \param image_file
//! \param options
Texture::Texture(std::filesystem::path image_file, TextureOptions options)
{
    loadFromFile(image_file.string(), options);
}
//! \brief constructs the texture from an \p image_file
//! \param image_file
//! \param options
Texture::Texture(const unsigned char *buffer, std::size_t size, TextureOptions options)
{
    loadFromBytes(buffer, size, options);
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

Texture::Texture(const Texture &other)
    : m_options(other.getOptions()), m_width(other.m_width), m_height(other.m_height)
{
    create(m_width, m_height, m_options);

    // Bind source texture to framebuffer
    GLuint fboSrc;
    glGenFramebuffers(1, &fboSrc);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboSrc);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, other.m_texture_handle, 0);

    // Bind destination texture to another framebuffer
    GLuint fboDst;
    glGenFramebuffers(1, &fboDst);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fboDst);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_handle, 0);

    // Copy the pixels
    glBlitFramebuffer(
        0, 0, m_width, m_height, // src rect
        0, 0, m_width, m_height, // dst rect
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST // or GL_LINEAR
    );

    // Cleanup
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fboSrc);
    glDeleteFramebuffers(1, &fboDst);

    glCopyTexImage2D(GL_TEXTURE_2D, 0,
                     static_cast<GLuint>(m_options.internal_format),
                     0, 0, m_width, m_height, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setMappingMinify(TexMappingParam map_min)
{
    bind();
    m_options.min_param = map_min;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGLCode(m_options.min_param));
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::setMappingMagnify(TexMappingParam map_mag)
{
    bind();
    m_options.mag_param = map_mag;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGLCode(m_options.mag_param));
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::setWrapX(TexWrapParam wrap_x)
{
    bind();
    m_options.wrap_x = wrap_x;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getGLCode(m_options.wrap_x));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setWrapY(TexWrapParam wrap_y)
{
    bind();
    m_options.wrap_y = wrap_y;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getGLCode(m_options.wrap_y));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::loadFromBytes(const unsigned char *buffer, std::size_t size, TextureOptions options)
{
    int channels_count = 0;
    // Load image from memory
    unsigned char *data = nullptr;
    stbi_set_flip_vertically_on_load(1);
    data = stbi_load_from_memory(buffer, size, &m_width, &m_height, &channels_count, 0);
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
        throw std::runtime_error("Error loading texture");
    }
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
    unsigned char *data = nullptr;
    int channels_count;
#ifdef __ANDROID__
    // On Android, open from assets
    SDL_RWops *rw = SDL_RWFromFile(filename.c_str(), "rb");
    if (!rw)
    {
        throw std::runtime_error("Failed to open texture " + filename + ": " + SDL_GetError());
    }

    // Read the entire file into memory
    Sint64 size = SDL_RWsize(rw);
    unsigned char *buffer = new unsigned char[size];
    Sint64 read_bytes = SDL_RWread(rw, buffer, 1, size);
    SDL_RWclose(rw);

    if (read_bytes != size)
    {
        delete[] buffer;
        throw std::runtime_error("Failed to read texture " + filename);
    }

    // Load image from memory
    data = stbi_load_from_memory(buffer, size, &m_width, &m_height, &channels_count, 0);
    delete[] buffer;
#else
    //! load texture from file
    data = stbi_load(filename.c_str(), &m_width, &m_height, &channels_count, 0);
#endif
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getGLCode(options.wrap_x));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getGLCode(options.wrap_y));
    glCheckError();

    assert(options.mag_param == TexMappingParam::Linear || options.mag_param == TexMappingParam::Nearest);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGLCode(options.min_param));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGLCode(options.mag_param));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, options.mipmap_levels);
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
    glTexImage2D(GL_TEXTURE_2D, 0, getGLCode(options.internal_format),
                 width, height, 0,
                 getGLCode(options.format),
                 getGLCode(options.data_type),
                 NULL);

                 if(options.mipmap_levels > 0)
                 {
                     glGenerateMipmap(GL_TEXTURE_2D);

                 }
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
        return false;
    }

    m_textures[texture_name] = std::make_shared<Texture>(texture);

    return true;
}

bool TextureHolder::add(std::string texture_name, std::filesystem::path texture_file_path, TextureOptions opt)
{
    if (m_textures.count(texture_name) != 0)
    {
        return false;
    }

    auto tex = std::make_shared<Texture>();
    tex->loadFromFile(texture_file_path.string(), opt);
    m_textures[texture_name] = std::move(tex);
    return true;
}
//! \brief reads the texture in \p texture_filename and adds it
//! \brief into the holder under id \p texture_name
//! \param texture_name our id of the texture
//! \param texture_filename     filename of the texture
//! \returns true if no texture of this name exists othrewise return false;
bool TextureHolder::add(std::string texture_name, std::string texture_filename, TextureOptions opt)
{
    return add(texture_name, m_resources_path / texture_filename, opt);
}

bool TextureHolder::add(std::string texture_name, const unsigned char *buffer, std::size_t size, TextureOptions opt)
{
    if (m_textures.count(texture_name) != 0)
    {
        return false;
    }

    auto tex = std::make_shared<Texture>();
    tex->loadFromBytes(buffer, size, opt);
    m_textures[texture_name] = std::move(tex);
    return true;
}

std::shared_ptr<Texture> TextureHolder::get(std::string name) const
{
    if (m_textures.count(name) > 0)
        return m_textures.at(name);

    return nullptr;
}

std::unordered_map<std::string, std::shared_ptr<Texture>> &TextureHolder::getTextures()
{
    return m_textures;
}

//! \brief sets base path for searching shaders when loading
//! \param directory    path to a directory
//! \returns true if the \p directory is actually an existing directory, otherwise returns false
bool TextureHolder::setBaseDirectory(std::filesystem::path directory)
{
    // if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    // {
    //     return false;
    // }

    m_resources_path = directory;
    return true;
}

const TextureOptions &Texture::getOptions() const
{
    return m_options;
}

void TextureHolder::erase(const std::string &texture_id)
{
    m_textures.erase(texture_id);
}

