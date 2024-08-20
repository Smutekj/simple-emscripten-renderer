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
