#include "FrameBuffer.h"


    FrameBuffer::FrameBuffer(int width, int height)
        : RenderTarget(width, height)
    {

        glGenFramebuffers(1, &m_target_handle);
        glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
        glCheckError();
        glViewport(0, 0, width, height);
        glCheckError();

        m_texture.create(width, height, GL_RGB16F, GL_RGB, GL_FLOAT);
        m_texture.bind();

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.getHandle(), 0); // texture, 0);//
        glCheckError();
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            throw std::runtime_error("FRAMEBUFFER NOT COMPLETE!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    Texture &FrameBuffer::getTexture()
    {
        return m_texture;
    }

    GLuint FrameBuffer::getHandle() const
    {
        return m_texture.getHandle();
    }
