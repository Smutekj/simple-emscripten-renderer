#include "FrameBuffer.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stbimage/stb_image_write.h"

#include <filesystem>

    FrameBuffer::FrameBuffer(int width, int height)
        : RenderTarget(width, height)
    {

        glGenFramebuffers(1, &m_target_handle);
        glBindFramebuffer(GL_FRAMEBUFFER, m_target_handle);
        glCheckError();
        glViewport(0, 0, width, height);
        glCheckError();

        TextureOptions options;
        m_texture.create(width, height, options) ;
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



 void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer)
{
    int width = buffer.getSize().x;
    int height = buffer.getSize().y;
    Image image(width, height);
    buffer.bind();
    glReadPixels(0, 0, width, height, GL_RGBA, GL_HALF_FLOAT, image.data());

    auto full_path = (path.string() + filename);
    int check = stbi_write_png(full_path.c_str(), width, height, 4, image.data(), 8 * width);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
}