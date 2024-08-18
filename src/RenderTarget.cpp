#include "RenderTarget.h"

RenderTarget::RenderTarget(int width, int height)
    : m_target_size(width, height)
{
}

utils::Vector2i RenderTarget::getSize() const
{
    return m_target_size;
}
void RenderTarget::bind()
{

    glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
    glCheckErrorMsg("hi");
    
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FRAMEBUFFER NOT COMPLETE!\n";
    }
}
void RenderTarget::clear(Color color)
{
    bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCheckErrorMsg("Hello");
    glClearColor(color.r, color.g, color.b, color.a);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCheckError();
}
