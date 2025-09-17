#pragma once

#include "Texture.h"
#include "RenderTarget.h"
#include "Color.h"

#include <vector>
#include <filesystem>

//! \class FrameBuffer
//! \brief manages the OpenGL FrameBuffer and it's corresponding bound texture
//!  used for off-screen rendering and stuff like bloom effect...
//!  also manages it's own texture, where we can access data resulted from drawing to the buffer
class FrameBuffer : public RenderTarget
{

public:
    FrameBuffer();
    FrameBuffer(int width, int height);
    FrameBuffer(int width, int height, TextureOptions options);

    ~FrameBuffer();
    FrameBuffer(const FrameBuffer &other) = default;
    FrameBuffer(FrameBuffer &&other) = default;
    FrameBuffer &operator=(const FrameBuffer &other) = default;
    FrameBuffer &operator=(FrameBuffer &&other) = default;

    Texture &getTexture();
    void setTexture(Texture &new_texture);
    GLuint getHandle() const;

private:
    std::shared_ptr<Texture> m_texture = nullptr;
    TextureOptions m_options;
};

//! \struct Image
//! \brief  holds texture data on the CPU
//!  probably useful for saving images to disk and graphics debugging
template <class PixelType>
struct Image
{

    Image(int x, int y);
    Image(Texture &tex_image);
    Image(FrameBuffer &tex_buffer);

    PixelType *data();
    int stride()const
    {
        return x_size * 4;
    }

    bool operator==(const Image<PixelType> &other_image) const;

    int getSizeX()const
    {
        return x_size;
    }
    int getSizeY()const
    {
        return y_size;
    }

    PixelType& at(std::size_t index)
    {
        return pixels.at(index);
    } 

private:
    void loadFromBuffer(FrameBuffer &tex_buffer);
    std::vector<PixelType> pixels;

private:
int x_size = 0;
int y_size = 0;
};

using LDRImage = Image<ColorByte>;
using HDRImage = Image<Color>;

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


void writeTextureToFile(std::filesystem::path path, std::string filename, Texture &buffer);
void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer);
