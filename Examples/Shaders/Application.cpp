#include "Application.h"

#include <IncludesGl.h>
#include <Utils/RandomTools.h>
#include <Utils/IOUtils.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <time.h>
#include <filesystem>

void drawProgramToTexture(Sprite &rect, Renderer &target, std::string program)
{
    target.clear({1, 1, 1, 1});
    target.drawSprite(rect, program, DrawType::Dynamic);
    target.drawAll();
}

Application::Application(int width, int height) : m_window(width, height),
                                                  m_window_renderer(m_window)
{

    std::filesystem::path resources_path = {__FILE__};
    resources_path.remove_filename().append("../Resources/Fonts/arial.ttf");
    m_font = std::make_shared<Font>(resources_path);
    glCheckErrorMsg("ERROR IN FONT CREATION!");

    int n_slots_x = 2;
    int n_slots_y = 2;
    m_slots.reserve(n_slots_x * n_slots_y);
    //! create slots for shaders
    for (int i = 0; i < n_slots_x * n_slots_y; ++i)
    {
        m_slots.emplace_back(width / n_slots_x, height / n_slots_y);
    }

    std::filesystem::path shaders_path = {__FILE__};
    shaders_path.remove_filename().append("../Resources/Shaders/");
    auto shader_filenames = extractNamesInDirectory(shaders_path, ".frag");
    for (auto &slot : m_slots)
    {
        slot.m_canvas.setShadersPath(shaders_path);
        for (auto &shader_filename : shader_filenames)
        {
            auto pos_right = shader_filename.find_last_of('.');
            std::string shader_name = shader_filename.substr(0, pos_right);
            slot.m_canvas.addShader(shader_name, "basicinstanced.vert", shader_filename);
        }

        slot.m_canvas.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    }

    m_window_renderer.setShadersPath(shaders_path);
    m_window_renderer.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    m_window_renderer.addShader("Text", "basicinstanced.vert", "textBorder.frag");
    m_window_renderer.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
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

    for (int slot_id = 0; slot_id < m_slots.size(); ++slot_id)
    {
        m_textures.add("Slot: " + std::to_string(slot_id), m_slots.at(slot_id).m_pixels.getTexture());
    }

    //! set view and add it to renderers
    m_view.setSize(m_window.getSize().x, m_window.getSize().y);
    m_view.setCenter(m_window.getSize().x / 2, m_window.getSize().y / 2);
    m_window_renderer.m_view = m_view;

    m_slots.at(0).m_selected_shader = "lightning";

    m_ui = std::make_unique<UI>(m_window, m_textures, m_slots);
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

void Application::handleInput()
{

    auto &imgui_io = ImGui::GetIO();

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        m_ui->handleEvent(event);
        if (imgui_io.WantCaptureMouse)
        {
            continue;
        }
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
    m_window.clear({1,1,1,1});

    if (m_wheel_is_held)
    {
        auto mouse_coords = m_window_renderer.getMouseInWorld();
        auto dr = mouse_coords - m_mouse_click_position;
        if (utils::norm(dr) > 0.5f)
        {
            moveView(dr, m_window_renderer);
            m_view = m_window_renderer.m_view;
        }
    }

    m_time += 0.016f;
    Shader::m_time = m_time;

    auto &shader_slot = m_slots.at(0);
    auto slot_size = shader_slot.getSize();
    int slot_ind = 0;
    for (auto &shader_slot : m_slots)
    {
        auto slot_size = shader_slot.getSize();

        Sprite test_sprite(shader_slot.m_pixels.getTexture());
        test_sprite.setScale(slot_size.x / 2.f, slot_size.y / 2.f);
        test_sprite.setPosition(slot_size.x / 2.f, slot_size.y / 2.f);
        if (!shader_slot.m_selected_shader.empty())
        {
            auto &shader = shader_slot.m_canvas.getShader(shader_slot.m_selected_shader);
            for (auto &[texture_name, texture_data] : shader.getVariables().textures)
            {
                test_sprite.m_texture_handles.at(texture_data.slot) = texture_data.handle;
            }
            shader_slot.m_canvas.m_view.setCenter(test_sprite.getPosition().x, test_sprite.getPosition().y);
            shader_slot.m_canvas.m_view.setSize(slot_size.x, slot_size.y);
            shader_slot.draw(test_sprite);
        }
        slot_ind++;
    }

    m_window_renderer.getShaders().refresh();
    glCheckError();

    int row = 0;
    int col = 0;
    float left_margin = 0;
    float top_margin = 0;

    for (auto &shader_slot : m_slots)
    {
        auto slot_size = shader_slot.getSize();

        Sprite screen_sprite(shader_slot.m_pixels.getTexture());
        screen_sprite.setScale(slot_size.x / 2.f, slot_size.y / 2.f);
        screen_sprite.setPosition(left_margin + slot_size.x / 2.f, top_margin + slot_size.y / 2.f);
        m_window_renderer.drawSprite(screen_sprite, "Instanced", DrawType::Dynamic);

        left_margin += slot_size.x;
        row++;
        if (row == 2)
        {
            row = 0;
            col++;
            left_margin = 0;
            top_margin += slot_size.y;
        }
    }
    m_window_renderer.m_view = m_view;
    m_window_renderer.drawAll();

    m_ui->draw(m_window);
    glCheckErrorMsg("Error in UI Draw!");
}

void inline gameLoop(void *mainLoopArg)
{
#ifdef __EMSCRIPTEN__
    // emscripten_trace_record_frame_start();
#endif
    auto tic = clock();
    // auto tic = std::chrono::high_resolution_clock::now();
    Application *p_app = (Application *)mainLoopArg;

    p_app->update(0);
    p_app->handleInput();

    // Swap front/back framebuffers
    SDL_GL_SwapWindow(p_app->m_window.getHandle());
    auto toc = clock();
    // auto toc =  std::chrono::high_resolution_clock::now();

    // std::cout << "frame took: " << std::chrono::duration_cast<std::chrono::microseconds>(toc - tic) << "\n";
    double dt = (double)(toc - tic) / CLOCKS_PER_SEC * 1000.f;
    // std::cout << "frame took: " << (dt) << "\n";
    SDL_Delay(10);
#ifdef __EMSCRIPTEN__
    // emscripten_trace_record_frame_end();
#endif
}
