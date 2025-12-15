#pragma once

#include "RenderTarget.h"


struct SDL_Window;

//! \class Window
//! \brief A class containing SDL handles to a window
class Window : public RenderTarget
{
public:
    Window(int width, int height);
    ~Window();

    SDL_Window *getHandle() const;
    void* getContext();

    bool shouldClose() const;
    void close();

    void setSize(int width, int height);
    void onResize();

private:
    bool m_should_close = false;
    SDL_Window *m_handle = nullptr;
};
