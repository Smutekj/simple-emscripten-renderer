#include "Window.h"

#include <iostream>


//! \brief constructs SDL window using it's \p width and \p height 
//! \param width
//! \param height
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
    
    // Get actual GL window size in pixels, in case of high dpi scaling
    utils::Vector2i size_check;
    SDL_GL_GetDrawableSize(m_handle, &size_check.x, &size_check.y);
    printf("INFO: GL window size = %dx%d\n", size_check.x, size_check.y);
    printf("INFO: Desired Window size = %dx%d\n", width, height);
 
    glViewport(0, 0, size_check.x, size_check.y);
}

Window::~Window()
{
    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_handle);
    SDL_Quit();
}

bool Window::shouldClose() const
{
    return m_should_close;
}

//! \returns an associated SDL handel of the window
SDL_Window *Window::getHandle() const
{
    return m_handle;
}


//! \returns an associated SDL_GLcontext
SDL_GLContext *Window::getContext()
{
    return &m_gl_context;
}

void Window::setSize(int width, int height)
{
    SDL_GetWindowSize(m_handle, &m_target_size.x, &m_target_size.y);
    auto surface = SDL_GetWindowSurface(m_handle);
    SDL_UpdateWindowSurface(m_handle);
}

//! \brief closes the window
void Window::close()
{
    m_should_close = true;
}