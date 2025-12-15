#include "RenderTarget.h"

#include "IncludesGl.h"

//! \brief constructs from width and height
//! \param width
//! \param height
RenderTarget::RenderTarget(int width, int height)
    : m_target_size(width, height)
{
}

utils::Vector2i RenderTarget::getSize() const
{
    return m_target_size;
}

//! \returns y/x aspect ratio
float RenderTarget::getAspect() const
{
    return static_cast<float>(m_target_size.y) / m_target_size.x;
}

//! \brief does GL calls to bind the target
void RenderTarget::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckErrorMsg("Error in Bind");
}

//! \brief clears the target Color and Depth buffers
//! \param  color      the new color of each pixel
void RenderTarget::clear(Color color)
{
    bind();
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCheckErrorMsg("DEPTH BUFFER clear MAY NOT BE SUPPORTED?");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
}
