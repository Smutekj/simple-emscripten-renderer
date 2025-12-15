#include "FrameBuffer.h"

#include "IncludesGl.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stbimage/stb_image_write.h"

#include <filesystem>

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &m_target_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckError();
}
FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_target_handle);
}

FrameBuffer::FrameBuffer(int width, int height, TextureOptions options)
    : RenderTarget(width, height), m_options(options)
{
    glGenFramebuffers(1, &m_target_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckError();
    glViewport(0, 0, width, height);
    glCheckError();

    m_texture = std::make_shared<Texture>();
    m_texture->create(width, height, m_options);
    m_texture->bind();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getHandle(), 0); 
    glCheckError();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("FRAMEBUFFER NOT COMPLETE!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//! \brief construct by specifying the buffer \p width and \p height
//! \param width
//! \param height
FrameBuffer::FrameBuffer(int width, int height)
    : FrameBuffer(width, height, {})
{
}

//! \returns a texture containing the buffer data on GPU
Texture &FrameBuffer::getTexture()
{
    assert(m_texture != nullptr);
    return *m_texture;
}

//! \brief No clue what I wanted to do with this :(
void FrameBuffer::setTexture(Texture &new_texture)
{
    m_target_size = new_texture.getSize();
    m_texture = std::shared_ptr<Texture>(&new_texture);
}

//! \return OpenGL handle of the associated texture
GLuint FrameBuffer::getHandle() const
{
    return m_texture->getHandle();
}


void FrameBuffer::resize(int w, int h)
{
    if(w == 0 || h == 0)
    {
        std::cout << "WARNING MAKING BUFFER WITH SIZE 0!" << std::endl;
        return;
    }
    m_texture = std::make_shared<Texture>();
    m_texture->create(w, h, m_options);
    m_texture->bind();

    m_target_size = m_texture->getSize();

    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getHandle(), 0);
    glCheckError();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("FRAMEBUFFER NOT COMPLETE!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void writeHDRTextureToFile(std::filesystem::path path, std::string filename, Texture &texture)
{
    int width = texture.getSize().x;
    int height = texture.getSize().y;
    Image<Color> image(texture);

    const Color *hdr_data = image.data();

    auto full_path = (path.string() + filename);
    int check = stbi_write_hdr(full_path.c_str(), width, height, 4, (float *)hdr_data);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
}

//! \brief for debugging
void writeTextureToFile(std::filesystem::path path, std::string filename, Texture &texture)
{
    if (texture.getOptions().data_type == TextureDataTypes::Float)
    {
        writeHDRTextureToFile(path, filename, texture);
        return;
    }

    int width = texture.getSize().x;
    int height = texture.getSize().y;
    Image<ColorByte> image(texture);

    int stride = 4 * width;
    auto full_path = (path.string() + filename);
    int check = stbi_write_png(full_path.c_str(), width, height, 4, image.data(), stride);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
}

void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer)
{
    writeTextureToFile(path, filename, buffer.getTexture());
}


template <class PixelType>
Image<PixelType>::Image(int x, int y)
    : x_size(x), y_size(y), pixels(x * y)
{
}

template <class PixelType>
PixelType *Image<PixelType>::data()
{
    return pixels.data();
}

template <class PixelType>
Image<PixelType>::Image(Texture &tex_image)
    : Image(tex_image.getSize().x, tex_image.getSize().y)
{
//! GLES3 does not have direct option of loading textures
#if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
    FrameBuffer texture_buffer(tex_image.getSize().x,
                               tex_image.getSize().y,
                               tex_image.getOptions());
    texture_buffer.setTexture(tex_image);
    loadFromBuffer(texture_buffer);
#else 
    if (tex_image.getOptions().data_type == TextureDataTypes::UByte)
    {
        glGetTextureImage(tex_image.getHandle(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                          4 * tex_image.getSize().x * tex_image.getSize().y, data());
    }
    else
    {
        glGetTextureImage(tex_image.getHandle(), 0, GL_RGBA, GL_FLOAT,
                          16 * tex_image.getSize().x * tex_image.getSize().y, data());
    }
#endif
    glCheckErrorMsg("Error in loading image from texture");
}

template <class PixelType>
Image<PixelType>::Image(FrameBuffer &tex_buffer)
    : Image(tex_buffer.getSize().x, tex_buffer.getSize().y)
{
    //! check that datatype in framebuffer is correct
    // assert(std::is_same_v<PixelType, Color> && tex_buffer.getO)
    loadFromBuffer(tex_buffer);
}

template <class PixelType>
bool Image<PixelType>::operator==(const Image<PixelType> &other_image) const
{

    for (size_t i = 0; i < other_image.pixels.size(); ++i)
    {
        if (pixels.at(i) != other_image.pixels.at(i))
        {
            return false;
        }
    }
    return true;
}

template <class PixelType>
void Image<PixelType>::loadFromBuffer(FrameBuffer &tex_buffer)
{
    assert(pixels.size() >= x_size * y_size);

    tex_buffer.bind();
    glReadPixels(0, 0, x_size, y_size,
                 static_cast<GLenum>(TextureFormat::RGBA),
                 static_cast<GLenum>(TextureDataTypes::UByte),
                 data());
    glCheckErrorMsg("Error in loading image from buffer");
}
