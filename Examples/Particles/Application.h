#pragma once


#include <chrono>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <memory>

#include <Window.h>
#include <Renderer.h>
#include <Font.h>
#include <DrawLayer.h>
#include <Particles.h>

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

    void initializeResources();
    void initializeLayers();
    void initializeUI();

    void drawUI();

    friend void gameLoop(void *);

private:
    Window m_window;
    Renderer m_window_renderer;
    TextureHolder m_textures;

    FrameBuffer m_scene_pixels;
    Renderer m_scene_canvas;

    View m_view;

    LayersHolder m_layers;

    utils::Vector2f m_old_view_center = m_view.getCenter();
    utils::Vector2f m_mouse_click_position = m_view.getCenter();

    std::string m_selected_layer_name = "BloomLayer";

    bool m_wheel_is_held = false;

    float m_time = 0.f;

    std::unique_ptr<Particles> m_particles;
    std::unique_ptr<TexturedParticles> m_tex_particles;

    Color m_init_color = {30, 0, 20, 1};
    Color m_final_color = {0, 10, 0, 1};
    Color m_background_color = {0, 0, 0, 1};

    // std::unique_ptr<UI> m_ui;
    std::shared_ptr<Font> m_font;
};