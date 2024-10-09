#pragma once

#include "RenderTarget.h"

#include <iostream>

class Window : public RenderTarget
{
public:
    Window(int width, int height);

    SDL_Window *getHandle() const;
    SDL_GLContext* getContext();

    bool shouldClose() const;
    void close();

    void setSize(int width, int height);

private:
    bool m_should_close = false;
    SDL_Window *m_handle;
    SDL_GLContext m_gl_context;
};
