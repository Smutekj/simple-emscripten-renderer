#include "Window.h"

#include <iostream>

#include <SDL.h>
#include <GLES/gl.h>

#include "IncludesGl.h"

#define LOG_CATEGORY SDL_LogCategory::SDL_LOG_CATEGORY_RENDER
#include "Utils/Logging.h"

static SDL_GLContext m_gl_context;

//! \brief constructs SDL window using it's \p width and \p height
//! \param width
//! \param height
Window::Window(int width, int height)
    : RenderTarget(width, height)
{
#if defined(DEBUG)
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_LogSetPriority(SDL_LogCategory::SDL_LOG_CATEGORY_RENDER, SDL_LogPriority::SDL_LOG_PRIORITY_DEBUG);
#else
    SDL_LogSetPriority(SDL_LogCategory::SDL_LOG_CATEGORY_RENDER, SDL_LogPriority::SDL_LOG_PRIORITY_INFO);
#endif

#if defined(ANDROID)
    SDL_SetHint(SDL_HINT_ORIENTATIONS,
                "Portrait PortraitUpsideDown LandscapeLeft LandscapeRight");
    SDL_InitSubSystem(SDL_INIT_VIDEO);
#endif

#if defined(ANDROID) || defined(EMSCRIPTEN)
    // Create OpenGL context on SDL window
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL window
    Uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
#if defined(ANDROID)
    window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_RESIZABLE;
#else
    window_flags |= SDL_WINDOW_RESIZABLE;
#endif

    m_handle =
        SDL_CreateWindow("Space Race",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height,
                         window_flags);

    m_gl_context = SDL_GL_CreateContext(m_handle);
#if !defined(ANDROID)
    if (SDL_GL_SetSwapInterval(1) == -1)
    {
        LOGI("VSYNC not supported!");
    }
#endif

#if defined(ANDROID)
     gladLoadGLES2((GLADloadfunc)SDL_GL_GetProcAddress);
#elif defined(EMSCRIPTEN)
    //! emscripten does it on it's own (I hope)
#else
    gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
#endif

    // bool multisampling_on;
    // glGetBooleanv(GL_MULTISAMPLE, &multisampling_on);
    // LOGI("Multisampling: %d", multisampling_on);
    glDisable(GL_MULTISAMPLE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    glCheckError();

    // Get actual GL window size in pixels, in case of high dpi scaling
    utils::Vector2i size_check;
    SDL_GL_GetDrawableSize(m_handle, &size_check.x, &size_check.y);
    GLint major, minor, profile;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    // glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    LOGI("INFO: GL window size = %dx%d\n", size_check.x, size_check.y);
    LOGI("INFO: Desired Window size = %dx%d\n", width, height);
    LOGI("GL Context: %d.%d profile=%d\n", major, minor, profile);
    LOGI("GL_VERSION: %s\n", glGetString(GL_VERSION));
    LOGI("GL_VENDOR   = %s\n", glGetString(GL_VENDOR));
    LOGI("GL_RENDERER = %s\n", glGetString(GL_RENDERER));

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
