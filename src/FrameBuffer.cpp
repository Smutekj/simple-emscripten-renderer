#include "FrameBuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stbimage/stb_image_write.h"

#include <filesystem>

FrameBuffer::FrameBuffer()
{
    glGenFramebuffers(1, &m_target_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckError();
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

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->getHandle(), 0); // texture, 0);//
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

// #ifndef __EMSCRIPTEN__
Image::Image(Texture &tex_image)
    : Image(tex_image.getSize().x, tex_image.getSize().y)
{
    loadFromTexture(tex_image);
}
// #endif

void Image::loadFromTexture(Texture &tex_image)
{
    // tex_image.bind(0);
    glGetTextureImage(tex_image.getHandle(), 0, GL_RGBA, GL_FLOAT,
                      16*tex_image.getSize().x * tex_image.getSize().y, data_hdr());
    glCheckErrorMsg("Error in loading image from texture");
    int i = 0;
}

Image::Image(FrameBuffer &tex_buffer)
    : Image(tex_buffer.getSize().x, tex_buffer.getSize().y)
{
    loadFromBuffer(tex_buffer);
}

bool Image::operator==(const Image &other_image) const
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

void Image::loadFromBuffer(FrameBuffer &tex_buffer)
{
    tex_buffer.bind();
    glReadPixels(0, 0, x_size, y_size,
                 static_cast<GLenum>(TextureFormat::RGBA),
                 static_cast<GLenum>(TextureDataTypes::UByte),
                 data());
    glCheckErrorMsg("Error in loading image from buffer");
}

//! \brief for debugging
void writeTextureToFile(std::filesystem::path path, std::string filename, Texture &texture)
{
    int width = texture.getSize().x;
    int height = texture.getSize().y;
    Image image(texture);

    std::vector<uint8_t> byte_image(4 * width * height);
    const Color* hdr_data = image.data_hdr();
    
    for (int i = 0; i < width * height ; ++i) {
        float clamped = std::max(0.0f, std::min(1.0f, hdr_data[i].r)); // clamp to [0,1]
        byte_image[4*i + 0] = static_cast<uint8_t>(clamped * 255.0f);
        clamped = std::max(0.0f, std::min(1.0f, hdr_data[i].g)); // clamp to [0,1]
        byte_image[4*i + 1] = static_cast<uint8_t>(clamped * 255.0f);
        clamped = std::max(0.0f, std::min(1.0f, hdr_data[i].b)); // clamp to [0,1]
        byte_image[4*i + 2] = static_cast<uint8_t>(clamped * 255.0f);
        clamped = std::max(0.0f, std::min(1.0f, hdr_data[i].a)); // clamp to [0,1]
        byte_image[4*i + 3] = static_cast<uint8_t>(clamped * 255.0f);
    }

    int stride = 4 * width;
    auto full_path = (path.string() + filename);
    int check = stbi_write_png(full_path.c_str(), width, height, 4, byte_image.data(), stride);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
}
void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer)
{
    int width = buffer.getSize().x;
    int height = buffer.getSize().y;
    Image image(buffer);

    int stride = 4 * width;
    auto full_path = (path.string() + filename);
    int check = stbi_write_png(full_path.c_str(), width, height, 4, image.data(), stride);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
}

Image::Image(int x, int y)
    : x_size(x), y_size(y), pixels(x * y), pixels_hdr(x * y)
{
}

ColorByte *Image::data()
{
    return pixels.data();
}
