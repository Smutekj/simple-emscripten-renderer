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
    Texture &getTexture();
    void setTexture(Texture& new_texture);
    GLuint getHandle() const;

private:
    std::shared_ptr<Texture> m_texture = nullptr;
    TextureOptions m_options;
};


//! \struct Image
//! \brief  holds texture data on the CPU
//!  probably useful for saving images to disk and graphics debugging
struct Image
{

    Image(int x, int y);

    Image(Texture& tex_image);

    Image(FrameBuffer &tex_buffer);
    
    ColorByte *data();
    Color *data_hdr()
    {
        return pixels_hdr.data();
    }

    bool operator==(const Image& other_image) const;
    
    private:
        void loadFromTexture(Texture &tex_buffer);
        void loadFromBuffer(FrameBuffer &tex_buffer);
    public:

    int x_size = 0;
    int y_size = 0;

    std::vector<Color> pixels_hdr;
    std::vector<ColorByte> pixels;
};

void writeTextureToFile(std::filesystem::path path, std::string filename, Texture &buffer);

void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer);
