#pragma once

#include <vector>
#include <numeric>
#include <unordered_map>
#include <memory>

#include <Window.h>
#include <Renderer.h>

#include <SDL_events.h>
#include <SDL_video.h>
#include <SDL_timer.h>

void gameLoop(void *mainLoopArg);

class Application
{
public:
    Application(int width, int height);

    void run();
    void update(float dt);
    void handleInput();

private:
    void onKeyPress(SDL_Keycode key);
    void onMouseButtonPress(SDL_MouseButtonEvent event);
    void onMouseButtonRelease(SDL_MouseButtonEvent event);
    void onKeyRelease(SDL_Keycode key);
    void onWheelMove(SDL_MouseWheelEvent event);

    void initializeUI();
    void drawUI();

    friend void gameLoop(void *);

private:
    Window m_window;
    Renderer m_window_renderer;
    TextureHolder m_textures;

    float m_ship_scale = 10.;
    ColorByte m_sprite_color = {255, 255, 255, 255};

    VertexArray m_meteor;
};