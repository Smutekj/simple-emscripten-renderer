#include <gtest/gtest.h>

#include <Shader.h>
#include "../CommonShaders.inl"

TEST(TestShaders, ShaderCompilation)
{
    int width = 800;
    int height = 600;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // Create SDL window
    SDL_Window *m_handle =
        SDL_CreateWindow("Test",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    auto m_gl_context = SDL_GL_CreateContext(m_handle);

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    Shader basic_instanced_shader(vertex_font_code, fragment_fullpass_code);

    basic_instanced_shader.use();
}
