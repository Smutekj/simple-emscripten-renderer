#include "Application.h"

#include <Utils/RandomTools.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <time.h>
#include <filesystem>

const std::filesystem::path shaders_path = {"../Resources/Shaders/"};
const std::filesystem::path textures_path = {"../Resources/Textures/"};

Application::Application(int width, int height) : m_window(width, height),
                                                  m_window_renderer(m_window)
{

    m_textures.setBaseDirectory(textures_path);
    m_textures.add("Ship", "EnemyLaser.png");

    m_window_renderer.m_view = m_window_renderer.getDefaultView();
    
    initializeUI();
}

void Application::run()
{

#ifdef __EMSCRIPTEN__
    int fps = 0; // Use browser's requestAnimationFrame
    emscripten_set_main_loop_arg(gameLoop, (void *)this, fps, true);
#else
    while (!m_window.shouldClose())
        gameLoop((void *)this);
#endif
}

void Application::drawUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Text");
    Color color = {m_sprite_color.r/255.f, m_sprite_color.g/255.f, m_sprite_color.b/255.f, m_sprite_color.a/255.f};
    ImGui::ColorPicker4("Sprite Color", &color.r);
    m_sprite_color = {color};
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::initializeUI()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    glCheckErrorMsg("Error in Imgui context creation");
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(m_window.getHandle(), m_window.getContext());
    ImGui_ImplOpenGL3_Init();
    glCheckErrorMsg("Error In ImGUI SDL2-Opengl context intiialization");

}

void Application::handleInput()
{

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            onKeyPress(event.key.keysym.sym);
            break;
        case SDL_KEYUP:
            onKeyRelease(event.key.keysym.sym);
            break;
        case SDL_MOUSEWHEEL:
            onWheelMove(event.wheel);
            break;
        case SDL_MOUSEBUTTONDOWN:
            onMouseButtonPress(event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseButtonRelease(event.button);
            break;
        default:
            break;
        }
    }
}

void Application::onKeyPress(SDL_Keycode key)
{
    switch (key)
    {
    }
}

void Application::onWheelMove(SDL_MouseWheelEvent event)
{

    if (event.preciseY < 0)
    {
        m_ship_scale *= 1.05f;
        m_ship_scale = std::max(0.f, m_ship_scale);
    }
    else if (event.preciseY > 0)
    {
        m_ship_scale /= 1.05f;
    }
}

void Application::onMouseButtonPress(SDL_MouseButtonEvent event)
{
}
void Application::onMouseButtonRelease(SDL_MouseButtonEvent event)
{
}

void Application::onKeyRelease(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_ESCAPE:
        m_window.close();
        break;
    }
}

void moveView(utils::Vector2f dr, Renderer &target)
{
    auto &view = target.m_view;
    auto old_view_center = view.getCenter();
    auto new_view_center = old_view_center - (dr);
    view.setCenter(new_view_center.x, new_view_center.y);
}

void Application::update(float dt)
{

    auto mouse_coords = m_window_renderer.getMouseInWorld();
    Shader::m_time += 0.016f;

    Sprite enemy_ship(*m_textures.get("Ship"));
    enemy_ship.setPosition(mouse_coords);
    enemy_ship.setScale(m_ship_scale, m_ship_scale);
    enemy_ship.setColor(m_sprite_color);

    m_window.clear({1, 1, 1, 1});
    m_window_renderer.drawSprite(enemy_ship);
    m_window_renderer.drawAll();

    drawUI();
}

void inline gameLoop(void *mainLoopArg)
{

    Application *p_app = (Application *)mainLoopArg;

    p_app->update(0);
    p_app->handleInput();

    // Swap front/back framebuffers
    SDL_GL_SwapWindow(p_app->m_window.getHandle());

    SDL_Delay(10);
}
