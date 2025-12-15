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

    void resize(int w, int h);

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

void writeTextureToFile(std::filesystem::path path, std::string filename, Texture &buffer);
void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer);
