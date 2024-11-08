#include "Window.h"

#include <iostream>

//! \brief constructs SDL window using it's \p width and \p height
//! \param width
//! \param height
Window::Window(int width, int height)
    : RenderTarget(width, height)
{
}

void Window::initialize(int width, int height)
{


    gl::Viewport(0, 0, m_target_size.x, m_target_size.y);
}

Window::~Window()
{
    // SDL_GL_DeleteContext(m_gl_context);
    // SDL_DestroyWindow(m_handle);
    // SDL_Quit();
}

bool Window::shouldClose() const
{
    return m_should_close;
}

// //! \returns an associated SDL handel of the window
// SDL_Window *Window::getHandle() const
// {
//     return m_handle;
// }

// //! \returns an associated SDL_GLcontext
// SDL_GLContext *Window::getContext()
// {
//     return &m_gl_context;
// }

// void Window::setSize(int width, int height)
// {
//     SDL_GetWindowSize(m_handle, &m_target_size.x, &m_target_size.y);
//     auto surface = SDL_GetWindowSurface(m_handle);
//     SDL_UpdateWindowSurface(m_handle);
// }

//! \brief closes the window
void Window::close()
{
    m_should_close = true;
}