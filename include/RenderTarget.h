#pragma once

#include "Vector2.h"
#include "Vertex.h"
#include "GLTypeDefs.h"

//! \class RenderTarget
//! \brief  OpenGL Target to draw into.
//! defines it's size in pixels and manages it's OpenGL handle
class RenderTarget
{

protected:
    RenderTarget(int width, int height);
    RenderTarget() = default;

public:
    utils::Vector2i getSize()const;
    void bind();
    float getAspect()const;

    //! \brief does not necessarily clear the currently bound RenderTarget!!! DO NOT FORGET!!!
    void clear(Color c);

protected:
    GLuint m_target_handle = 0; //!< gl id of the target (default is the window)
    utils::Vector2i m_target_size = {0,0}; 
};
