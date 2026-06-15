#include "FrameBuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stbimage/stb_image_write.h"

#include "IncludesGl.h"
#include "Utils/Logging.h"

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &m_target_handle);
    glCheckError();
}
FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_target_handle);
}

FrameBuffer::FrameBuffer(int width, int height, TextureOptions options, int sample_count)
    : RenderTarget(width, height), m_options(options), m_msaa_textures(sample_count)
{
    glGenFramebuffers(1, &m_target_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckError();
    glViewport(0, 0, width, height);
    glCheckError();

    m_texture = std::make_shared<Texture>();
    m_texture->create(width, height, m_options);
    m_texture->bind();

    // glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER,
    //                                      GL_COLOR_ATTACHMENT0,
    //                                      GL_TEXTURE_2D,
    //                                      m_texture->getHandle(),
    //                                      0, 1);

    //! we want to use MSAA
    // if (sample_count > 1)
    // {
    //     glGenTextures(sample_count, m_msaa_textures.data());
    //     for (int image_id = 0; image_id < sample_count; ++image_id)
    //     {
    //         glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_msaa_textures[image_id]);
    //         // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, sample_count, GL_RGB, width, height, GL_TRUE);
    //         glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    //     }
    // }

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           m_texture->getHandle(),
                           0);

    glCheckError();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("FRAMEBUFFER NOT COMPLETE!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer(std::shared_ptr<Texture> p_texture)
    : RenderTarget(p_texture->getSize().x, p_texture->getSize().y),
      m_texture(p_texture)
{
    glGenFramebuffers(1, &m_target_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckError();
    glViewport(0, 0, p_texture->getSize().x, p_texture->getSize().y);

    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getHandle(), 0, 1);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getHandle(), 0);
    glCheckError();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        throw std::runtime_error("FRAMEBUFFER NOT COMPLETE!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//! \returns a texture containing the buffer data on GPU
Texture &FrameBuffer::getTexture()
{
    assert(m_texture != nullptr);
    return *m_texture;
}

//! \brief No clue what I wanted to do with this :(
void FrameBuffer::setTexture(std::shared_ptr<Texture> new_texture)
{
    m_target_size = utils::Vector2i{new_texture->getSize()};
    m_texture = new_texture;
}

//! \return OpenGL handle of the associated texture
GLuint FrameBuffer::getHandle() const
{
    return m_texture->getHandle();
}

void FrameBuffer::resize(int w, int h)
{
    if (w == 0 || h == 0)
    {
        std::cout << "WARNING MAKING BUFFER WITH SIZE 0!" << std::endl;
        return;
    }
    m_texture = std::make_shared<Texture>();
    m_texture->create(w, h, m_options);
    m_texture->bind();

    m_target_size = utils::Vector2i{m_texture->getSize()};

    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    // glFramebufferTexture2DMultisampleEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getHandle(), 0, 1);
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

std::size_t writeTextureToFile(std::ofstream &stream, Texture& texture)
{
    if (texture.getOptions().data_type == TextureDataTypes::Float)
    {
        return 0;
    }

    int width = texture.getSize().x;
    int height = texture.getSize().y;
    Image<ColorByte> image(texture);

    int stride_bytes = 4 * width;
    int len;
    unsigned char *png = stbi_write_png_to_mem((const unsigned char *)image.data(), stride_bytes, width, height, 4, &len);
    if (png == NULL)
    {
        return 0;
    }
    stream.write((const char *)png, len);

    STBIW_FREE(png);
    return len;
}

void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer)
{
    TextureOptions options = buffer.getTexture().getOptions();
    if (options.data_type == TextureDataTypes::Float)
    {
        writeHDRTextureToFile(path, filename, buffer.getTexture());
        return;
    }

    int width = buffer.getSize().x;
    int height = buffer.getSize().y;
    Image<ColorByte> image(buffer);

    int stride = 4 * width;
    auto full_path = (path.string() + filename);
    int check = stbi_write_png(full_path.c_str(), width, height, 4, image.data(), stride);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
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
Image<PixelType>::Image(Texture& tex_image)
    : Image(tex_image.getSize().x, tex_image.getSize().y)
{
//! GLES3 does not have direct option of loading textures (do this without FameBuffer!!!!)
#if defined(GLES)
    LOGW("Can't load image from texture in GLES! Use Framebuffer version instead!");
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
    glCheckErrorMsg("Error in loading image from texture");
#endif
}

template <class PixelType>
Image<PixelType>::Image(FrameBuffer &tex_buffer)
    : Image(tex_buffer.getSize().x, tex_buffer.getSize().y)
{
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

    glCheckErrorMsg("Error in loading image from buffer");
    tex_buffer.bind();
    glReadPixels(0, 0, x_size, y_size,
                 getGLCode(TextureFormat::RGBA),
                 getGLCode(TextureDataTypes::UByte),
                 data());
    glCheckErrorMsg("Error in loading image from buffer");
}
