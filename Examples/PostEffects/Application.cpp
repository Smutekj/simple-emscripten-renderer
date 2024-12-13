#include "Application.h"

#include <IncludesGl.h>
#include <Utils/RandomTools.h>
#include <Utils/IOUtils.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <time.h>
#include <filesystem>

void Application::initializeLayers()
{
    std::filesystem::path shaders_path =  {__FILE__};
    shaders_path.remove_filename().append("../Resources/Shaders/");

    auto &base_layer = m_layers.addLayer("BaseLayer", 0);
    base_layer.m_canvas.setShadersPath(shaders_path);
    base_layer.m_canvas.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    base_layer.m_canvas.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    auto &upper_layer = m_layers.addLayer("BloomLayer", 5);
    upper_layer.m_canvas.setShadersPath(shaders_path);
    upper_layer.m_canvas.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    upper_layer.m_canvas.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    upper_layer.addEffect(std::make_unique<Bloom3>(m_window_renderer.getTargetSize().x, m_window_renderer.getTargetSize().y));
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
    std::filesystem::path resources_path = {__FILE__};
    resources_path.remove_filename().append("../Resources/Fonts/arial.ttf");
    m_font = std::make_shared<Font>(resources_path);
    glCheckErrorMsg("ERROR IN FONT CREATION!");

    std::filesystem::path shaders_path = {__FILE__};
    shaders_path.remove_filename().append("../Resources/Shaders/");

    m_scene_canvas.setShadersPath(shaders_path);
    m_scene_canvas.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    m_scene_canvas.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    m_window_renderer.setShadersPath(shaders_path);
    m_window_renderer.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    m_window_renderer.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    m_window_renderer.addShader("LastPass", "basicinstanced.vert", "lastPass.frag");
    glCheckErrorMsg("Error in Shaders creation!");

    std::filesystem::path textures_path = {__FILE__};
    textures_path.remove_filename().append("../Resources/Textures/");
    auto texture_filenames = extractNamesInDirectory(textures_path, ".png");
    m_textures.setBaseDirectory(textures_path);
    for (auto &texture_filename : texture_filenames)
    {
        auto pos_right = texture_filename.find_last_of('.');
        std::string texture_name = texture_filename.substr(0, pos_right);
        auto status = m_textures.add(texture_name, texture_filename);
    }
}

Application::Application(int width, int height) : m_window(width, height),
                                                  m_window_renderer(m_window),
                                                  m_scene_pixels(width, height),
                                                  m_scene_canvas(m_scene_pixels)
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

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({400, 200});
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

    ImGui::SetNextWindowPos({0, 200});
    ImGui::SetNextWindowSize({400, 350});
    ImGui::Begin("Colors");
    ImGui::InputFloat4("Sprite Color", &m_rect_color.r);
    ImGui::ColorPicker4("Background", &m_background_color.r);
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::handleInput()
{

    // auto &imgui_io = ImGui::GetIO();

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
    m_window.clear(m_background_color);

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

    auto *canvas2 = m_layers.getCanvasP(m_selected_layer_name);
    if (canvas2)
    {
        RectangleSimple rect;
        rect.setPosition(m_window_renderer.getDefaultView().getCenter());
        rect.setScale(m_window_renderer.getDefaultView().getSize() / 8.f);
        canvas2->drawRectangle(rect, m_rect_color);
    }

    m_scene_canvas.clear(m_background_color);
    m_layers.setView(m_view);
    m_layers.clearAllLayers();
    m_layers.drawInto(m_scene_canvas);

    m_window_renderer.clear(m_background_color);
    auto scene_size = m_scene_pixels.getSize();
    auto old_view = m_window_renderer.m_view;
    Sprite screen_sprite(m_scene_pixels.getTexture());
    screen_sprite.setPosition(scene_size / 2.f);
    screen_sprite.setScale(scene_size / 2.f);
    m_window_renderer.m_view.setCenter(screen_sprite.getPosition());
    m_window_renderer.m_view.setSize(scene_size);
    m_window_renderer.drawSprite(screen_sprite, "LastPass", DrawType::Dynamic);
    auto old_factors = m_window_renderer.m_blend_factors;
    m_window_renderer.m_blend_factors = {BlendFactor::One, BlendFactor::OneMinusSrcAlpha};
    m_window_renderer.drawAll();
    m_window_renderer.m_blend_factors = old_factors;
    m_window_renderer.m_view = old_view;

    drawUI();
}

void inline gameLoop(void *mainLoopArg)
{
    auto tic = clock();
    // auto tic = std::chrono::high_resolution_clock::now();
    Application *p_app = (Application *)mainLoopArg;

    p_app->update(0);
    p_app->handleInput();

    // Swap front/back framebuffers
    SDL_GL_SwapWindow(p_app->m_window.getHandle());
    auto toc = clock();

    double dt = (double)(toc - tic) / CLOCKS_PER_SEC * 1000.f;
    SDL_Delay(10);
}
