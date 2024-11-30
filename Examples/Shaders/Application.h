#pragma once


#include "ShaderUI.h"

#include <chrono>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <memory>

#include <Window.h>
#include <Renderer.h>
#include <Font.h>

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

    friend void gameLoop(void *);

private:
    Window m_window;
    View m_view;

    utils::Vector2f m_old_view_center = m_view.getCenter();
    utils::Vector2f m_mouse_click_position = m_view.getCenter();

    bool m_wheel_is_held = false;

    Renderer m_window_renderer;
    TextureHolder m_textures;
    
    float m_time = 0.f;

    std::unique_ptr<UI> m_ui;
    std::vector<ShaderSlot> m_slots;
    std::shared_ptr<Font> m_font;  
};