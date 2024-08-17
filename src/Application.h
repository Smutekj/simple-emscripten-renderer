#pragma once

#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "ShaderUI.h"

#include <chrono>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <memory>

#include "Rectangle.h"
#include "VertexArray.h"
#include "View.h"
#include "FrameBuffer.h"
#include "Renderer.h"
#include "Particles.h"

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
    void initializeSimulation();
    void doBloom(Texture& source, Renderer& target);

    friend void gameLoop(void *);

private:
    Window m_window;
    View m_view;

    cdt::Vector2f m_old_view_center = m_view.getCenter();
    cdt::Vector2f m_mouse_click_position = m_view.getCenter();

    Shader shader = {"../Resources/basic.vert", "../Resources/fullpass.frag"};
    Shader shadertex = {"../Resources/basictex.vert", "../Resources/texture.frag"};
    Texture texture;
    Rectangle rect = {shader};

    bool m_wheel_is_held = false;
    bool m_is_moving_right = false;
    bool m_is_moving_left = false;
    bool m_is_moving_up = false;
    bool m_is_moving_down = false;

    Renderer m_window_renderer;
    TextureHolder m_textures;
    
    VertexArray m_va = {shadertex};

    float m_time = 0.f;

    bool m_simulation_on = false;
    FrameBuffer b1;
    Renderer m_swirl_renderer1;
    Renderer m_vel_initializer;
    FrameBuffer m_vel_init_texture;

    std::unique_ptr<Particles> m_particles;
    std::unique_ptr<Particles> m_particles2;
    std::unique_ptr<Particles> m_particles3;
    Color m_particle_color = {0.2, 2.0, 4.0};

    std::chrono::high_resolution_clock::time_point tic;

    std::unique_ptr<UI> m_ui;
    std::vector<ShaderSlot> m_slots;
    int m_simulation_slot = 0;

    Renderer m_bloom_renderer1;
    Renderer m_bloom_renderer2;
    FrameBuffer m_bloom_pass1;
    FrameBuffer m_bloom_pass2;
    

};