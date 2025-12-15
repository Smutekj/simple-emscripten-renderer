#include "Application.h"

#include <Utils/RandomTools.h>
#include <Utils/IOUtils.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <time.h>
#include <filesystem>


const std::filesystem::path shaders_path = {"../Resources/Shaders/"};


void Application::initializeLayers()
{
    auto &lower_layer = m_layers.addLayer("Layer1", 5);
    lower_layer.m_canvas.setShadersPath(shaders_path);
    auto &upper_layer = m_layers.addLayer("Layer2", 0);
    upper_layer.m_canvas.setShadersPath(shaders_path);
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

void Application::initializeResources()
{
    std::filesystem::path resources_path = {"../Resources/Fonts/arial.ttf"};
    m_font = std::make_shared<Font>(resources_path);
    glCheckErrorMsg("ERROR IN FONT CREATION!");


    m_window_renderer.setShadersPath(shaders_path);
    m_window_renderer.addShader("Text", "basicinstanced.vert", "textBorder.frag");
    glCheckErrorMsg("Error in Shaders creation!");

    std::filesystem::path textures_path = {"../Resources/Textures/"};
    auto texture_filenames = extractNamesInDirectory(textures_path, ".png");
    m_textures.setBaseDirectory(textures_path);
    for (auto &texture_filename : texture_filenames)
    {
        auto pos_right = texture_filename.find_last_of('.');
        std::string texture_name = texture_filename.substr(0, pos_right);
        m_textures.add(texture_name, texture_filename);
    }
}

Application::Application(int width, int height) : m_window(width, height),
                                                  m_window_renderer(m_window)
{

    initializeResources();
    initializeLayers();
    initializeUI();

    //! set view and add it to renderers
    m_view.setSize(m_window.getSize().x, m_window.getSize().y);
    m_view.setCenter(m_window.getSize().x / 2, m_window.getSize().y / 2);
    m_window_renderer.m_view = m_view;

    glCheckErrorMsg("Error in UI Creation");
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

    ImGui::SetNextWindowPos({0,0});
    ImGui::SetNextWindowSize({400,200}); 
    ImGui::Begin("Layers");
    for (auto [layer_name, depth] : m_layers.m_name2depth)
    {
        auto &layer = m_layers.m_layers.at(depth);
        if (ImGui::TreeNode(layer_name.c_str()))
        {
            if (ImGui::Button("Toggle Active"))
            {
                layer->toggleActivate();
            }
            if (ImGui::InputInt("Depth", &depth))
            {
                m_layers.changeDepth(layer_name, depth);
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();

    ImGui::SetNextWindowPos({0,200});
    ImGui::SetNextWindowSize({400,350}); 
    ImGui::Begin("Colors");
    ImGui::ColorPicker4("Rectangle Color", &m_rect_color.r);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::handleInput()
{

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        //! tell ui event happened
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
        m_view.zoom(0.95f);
    }
    else if (event.preciseY > 0)
    {

        m_view.zoom(1. / 0.95f);
    }
}

void Application::onMouseButtonPress(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_MIDDLE)
    {
        m_wheel_is_held = true;
        m_old_view_center = m_view.getCenter();
        m_mouse_click_position = m_window_renderer.getMouseInWorld();
    }
}
void Application::onMouseButtonRelease(SDL_MouseButtonEvent event)
{
    if (event.button == SDL_BUTTON_MIDDLE)
    {
        m_wheel_is_held = false;
    }
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
    m_window.clear({1, 1, 1, 1});

    auto mouse_coords = m_window_renderer.getMouseInWorld();
    if (m_wheel_is_held)
    {
        auto dr = mouse_coords - m_mouse_click_position;
        if (utils::norm(dr) > 0.5f)
        {
            moveView(dr, m_window_renderer);
            m_view = m_window_renderer.m_view;
        }
    }

    m_time += 0.016f;
    Shader::m_time = m_time;

    //! we use pointer because a user may screw up a name in string;
    auto *canvas1 = m_layers.getCanvasP("Layer1");
    if (canvas1)
    {
        RectangleSimple rect1(m_rect_color);
        rect1.setPosition(mouse_coords);
        rect1.setScale(m_window_renderer.getDefaultView().getSize() / 8.f);
        canvas1->m_blend_factors = {BlendFactor::SrcAlpha, BlendFactor::OneMinusSrcAlpha};
        canvas1->drawRectangle(rect1);
    }

    auto *canvas2 = m_layers.getCanvasP("Layer2");
    if (canvas2)
    {
        Sprite ship(*m_textures.get("EnemyLaser"));
        ship.setPosition(m_window_renderer.getDefaultView().getCenter());
        ship.setScale(m_window_renderer.getDefaultView().getSize() / 8.f);
        canvas2->drawSprite(ship);
    }

    m_window_renderer.clear({255, 255, 255, 255});
    m_layers.setView(m_view);
    m_layers.clearAllLayers();
    m_layers.drawInto(m_window_renderer);

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
