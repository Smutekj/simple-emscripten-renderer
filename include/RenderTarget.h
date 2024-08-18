#pragma once

#include "Vector2.h"
#include "Vertex.h"
#include "IncludesGl.h"

class RenderTarget
{

protected:
    RenderTarget(int width, int height);

public:
    utils::Vector2i getSize()const;
    void bind();
    void clear(Color c);

protected:
    GLuint m_target_handle = 0; //! gl id of the target (default is the window)
    utils::Vector2i m_target_size; 
};
