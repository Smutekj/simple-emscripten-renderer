#pragma once

#include "Texture.h"
#include "RenderTarget.h"

class FrameBuffer : public RenderTarget
{

public:
    FrameBuffer(int width, int height);
    Texture &getTexture();
    GLuint getHandle() const;

private:
    Texture m_texture;
};
