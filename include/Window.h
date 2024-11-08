#pragma once

#include "RenderTarget.h"
#include "EglContext.h"

#include <iostream>

//! \class Window
//! \brief A class containing SDL handles to a window
class Window : public RenderTarget
{
public:
    Window(int width, int height);
    void initialize(int width, int height);

    ~Window();

    // SDL_Window *getHandle() const;
    // SDL_GLContext* getContext();

    void swapBuffers();

    bool shouldClose() const;
    void close();

    void setSize(int width, int height);
    
    EglContext m_egl_context;

private:
    bool m_should_close = false;

    // SDL_Window *m_handle = nullptr;
    // SDL_GLContext m_gl_context = nullptr;
};
