#pragma once

#include "Texture.h"
#include "RenderTarget.h"

#include <vector>
#include <filesystem>

class FrameBuffer : public RenderTarget
{

public:
    FrameBuffer(int width, int height);
    Texture &getTexture();
    GLuint getHandle() const;

private:
    Texture m_texture;
    GLint m_internal_format = GL_RGB16F;
    GLint m_format = GL_RGBA;
    GLint m_channel_format = GL_HALF_FLOAT;
};

struct ColorByte
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;
};

struct Image
{

    Image(int x, int y)
        : x_size(x), y_size(y), pixels(x * y)
    {
    }

    ColorByte *data()
    {
        return pixels.data();
    }

    int x_size;
    int y_size;

    std::vector<ColorByte> pixels;
};


void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer);
