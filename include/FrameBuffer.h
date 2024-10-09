#pragma once

#include "Texture.h"
#include "RenderTarget.h"
#include "Color.h"

#include <vector>
#include <filesystem>


//! \class manages to OpenGL FrameBuffer
//! \brief used for off-screen rendering and stuff like bloom effect...
//! \brief also manages it's own texture, where we can access data resulted from drawing to the buffer
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


//! \struct holds texture data on the CPU
//! \brief probably useful for saving images to disk
struct Image
{

    Image(int x, int y);

    ColorByte *data();

    int x_size = 0;
    int y_size = 0;

    std::vector<ColorByte> pixels;
};

void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer);
