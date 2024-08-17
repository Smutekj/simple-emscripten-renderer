#include "Window.h"

#include <iostream>

Window::Window(int width, int height)
    : RenderTarget(width, height)
{

    // Create SDL window
    m_handle =
        SDL_CreateWindow("Hello Triangle Minimal",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

    // Create OpenGLES 3 context on SDL window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG); //! DO NOT USE WITH EMSCRIPTEN!!!
    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    m_gl_context = SDL_GL_CreateContext(m_handle);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    glCheckError();

    printf("INFO: GL version: %s\n", glGetString(GL_VERSION));

    // Set clear color to black
    // glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Get actual GL window size in pixels, in case of high dpi scaling
    cdt::Vector2i size_check;
    SDL_GL_GetDrawableSize(m_handle, &size_check.x, &size_check.y);
    printf("INFO: GL window size = %dx%d\n", size_check.x, size_check.y);
    printf("INFO: Desired Window size = %dx%d\n", width, height);
    glViewport(0, 0, size_check.x, size_check.y);
}

bool Window::shouldClose() const
{
    return m_should_close;
}

SDL_Window *Window::getHandle() const
{
    return m_handle;
}

SDL_GLContext *Window::getContext()
{
    return &m_gl_context;
}