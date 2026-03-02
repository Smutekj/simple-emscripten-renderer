#include "Window.h"

#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>
#include "IncludesGl.h"

SDL_GLContext m_gl_context;

//! \brief constructs SDL window using it's \p width and \p height
//! \param width
//! \param height
Window::Window(int width, int height)
    : RenderTarget(width, height)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

#if defined(ANDROID)
    // Create OpenGL context on SDL window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif

    SDL_GL_SetSwapInterval(1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL window
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#if defined(ANDROID)
    window_flags |= SDL_WINDOW_FULLSCREEN;
#else
    window_flags |= SDL_WINDOW_RESIZABLE;
#endif
    m_handle =
        SDL_CreateWindow("Space Race",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height,
                         window_flags);

    m_gl_context = SDL_GL_CreateContext(m_handle);

#if defined(ANDROID)
    gladLoadGLES2Loader((GLADloadproc)SDL_GL_GetProcAddress);
#else
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
#endif
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
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

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
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
void *Window::getContext()
{
    return &m_gl_context;
}

void Window::setSize(int width, int height)
{
    SDL_SetWindowSize(m_handle, width, height);
    onResize();
}
void Window::onResize()
{
    SDL_GetWindowSize(m_handle, &m_target_size.x, &m_target_size.y);
    glViewport(0, 0, m_target_size.x, m_target_size.y);
}

//! \brief closes the window
void Window::close()
{
    m_should_close = true;
}
