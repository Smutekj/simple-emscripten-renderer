#include "RenderTarget.h"

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

//! \brief does GL calls to bind the target
void RenderTarget::bind()
{
    gl::BindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckErrorMsg("hi");
    
    auto status = gl::CheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FRAMEBUFFER NOT COMPLETE!\n";
    }
}

//! \brief clears the target Color and Depth buffers 
//! \param  color      the new color of each pixel
void RenderTarget::clear(Color color)
{
    bind();
    gl::ClearColor(color.r, color.g, color.b, color.a);
    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCheckErrorMsg("DEPTH BUFFER clear MAY NOT BE SUPPORTED?");
    gl::BindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
}
