#pragma once

#include "Texture.h"
#include "RenderTarget.h"

#include <vector>
#include <filesystem>

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

    int x_size = 0;
    int y_size = 0;

    std::vector<ColorByte> pixels;
};

void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer);
