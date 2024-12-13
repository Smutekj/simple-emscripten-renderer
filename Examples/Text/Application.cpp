#include "Application.h"

#include <Utils/RandomTools.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <time.h>
#include <filesystem>

Application::Application(int width, int height) : m_window(width, height),
                                                  m_window_renderer(m_window)
{

    std::filesystem::path font_path = {__FILE__};
    font_path.remove_filename().append("../../Resources/Fonts/arial.ttf");
    m_font = std::make_shared<Font>(font_path);

    std::filesystem::path shaders_path = {__FILE__};
    shaders_path.remove_filename().append("../../Resources/Shaders/");
    m_window_renderer.setShadersPath(shaders_path);
    m_window_renderer.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    m_window_renderer.addShader("Text", "basicinstanced.vert", "textBorder.frag");
    m_window_renderer.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    
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
    ImGui::InputText("Text", m_screen_text.data(), 100);

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

    auto &imgui_io = ImGui::GetIO();

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
        m_window_renderer.m_view.zoom(0.95f);
    }
    else if (event.preciseY > 0)
    {

        m_window_renderer.m_view.zoom(1. / 0.95f);
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

    Shader::m_time += 0.016f;

    Text test_text(m_screen_text);
    test_text.setFont(m_font.get());
    test_text.setPosition(400, 300);
    test_text.setScale(1, 1);
    test_text.setColor({255, 0, 255, 255});

    auto mouse_coords = m_window_renderer.getMouseInWorld();
    Sprite test_sprite(m_font->getTexture()); //*m_textures.get("arrow"));
    test_sprite.m_color = {0, 0, 0, 255};
    test_sprite.setScale(400, 300);
    test_sprite.setPosition(400, 300);
    m_window.clear({1, 1, 1, 1});
    m_window_renderer.drawSprite(test_sprite, "Text", DrawType::Dynamic);
    test_text.setPosition(mouse_coords);
    m_window_renderer.drawText(test_text, "Text", DrawType::Dynamic);
    m_window_renderer.drawAll();

    drawUI();
}

void inline gameLoop(void *mainLoopArg)
{

    auto tic = clock();
    Application *p_app = (Application *)mainLoopArg;

    p_app->update(0);
    p_app->handleInput();

    // Swap front/back framebuffers
    SDL_GL_SwapWindow(p_app->m_window.getHandle());
    auto toc = clock();

    double dt = (double)(toc - tic) / CLOCKS_PER_SEC * 1000.f;
    // std::cout << "frame took: " << (dt) << "\n";
    SDL_Delay(10);
}
