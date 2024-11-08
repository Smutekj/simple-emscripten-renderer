#include "Application.h"

#include <IncludesGl.h>
#include <Utils/RandomTools.h>
#include <Utils/IO.h>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#include <time.h>
#include <filesystem>

template <class UniformType>
struct ShaderUniform
{
    std::string name;
    UniformType value;
};

// void Application::initializeSimulation()
// {
//     m_swirl_renderer1.addShader("swirl", "basicinstanced.vert", "swirl.frag");
//     m_vel_initializer.addShader("SwirlInit", "basicinstanced.vert", "velFieldInit.frag");

//     auto &simulation_canvas = m_slots.at(0).m_canvas;
//     auto &simulation_pixels = m_slots.at(0).m_pixels;
//     //! draw initial condition to buffer 2
//     Sprite buffer_Sprite(b1.getTexture());
//     utils::Vector2f slot_size = {simulation_pixels.getSize().x, simulation_pixels.getSize().y};
//     buffer_Sprite.setPosition(slot_size.x / 2.f, slot_size.y / 2.f);
//     buffer_Sprite.setScale(slot_size.x / 2.f, slot_size.y / 2.f);
//     simulation_canvas.m_view.setCenter(buffer_Sprite.getPosition().x, buffer_Sprite.getPosition().y);
//     simulation_canvas.m_view.setSize(slot_size.x, slot_size.y);
//     simulation_canvas.drawSprite(buffer_Sprite, "rhoFieldInit", GL_DYNAMIC_DRAW);
//     simulation_canvas.drawAll();

//     m_vel_initializer.m_view.setCenter(buffer_Sprite.getPosition().x, buffer_Sprite.getPosition().y);
//     m_vel_initializer.m_view.setSize(slot_size.x, slot_size.y);
//     m_vel_initializer.drawSprite(buffer_Sprite, "SwirlInit", GL_DYNAMIC_DRAW);
//     m_vel_initializer.drawAll();
// }

template <class... UniformType>
void setUniforms(Shader &program, ShaderUniform<UniformType> &...values)
{
    (program.setUniform(values.name, values.value), ...);
}

void drawProgramToTexture(Sprite &rect, Renderer &target, std::string program)
{
    target.clear({1, 1, 1, 1});
    target.drawSprite(rect, program, DrawType::Dynamic);
    target.drawAll();
}

Application::Application(int width, int height) : m_window(width, height),
                                                  m_window_renderer(m_window)
{

    m_test_font = std::make_shared<Font>("arial.ttf");
    
    std::filesystem::path path{"../Resources/Shaders"};
    auto shader_filenames = extractNamesInDirectory(path, ".frag");
    for (auto &shader_filename : shader_filenames)
    {
        auto pos_right = shader_filename.find_last_of('.');
        std::string shader_name = shader_filename.substr(0, pos_right);
        for (auto &slot : m_slots)
        {
            slot.m_canvas.addShader(shader_name, "basicinstanced.vert", path.string() + shader_filename);
        }
    }

    for (auto &slot : m_slots)
    {
        slot.m_canvas.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");
    }

    m_window_renderer.addShader("Instanced", "basicinstanced.vert", "texture.frag");
    m_window_renderer.addShader("Text", "basicinstanced.vert", "textBorder.frag");
    m_window_renderer.addShader("VertexArrayDefault", "basictex.vert", "fullpass.frag");


    auto texture_filenames = extractNamesInDirectory(path, ".png");
    for (auto &texture_filename : texture_filenames)
    {
        auto pos_right = texture_filename.find_last_of('.');
        std::string texture_name = texture_filename.substr(0, pos_right);
        auto status = m_textures.add(texture_name, "" + texture_filename);
    }
    int slot_id = 0;
    for (auto &slot : m_slots)
    {
        m_textures.add("Slot: " + std::to_string(slot_id), slot.m_pixels.getTexture());
        slot_id++;
    }

    //! set view and add it to renderers
    m_view.setSize(m_window.getSize().x, m_window.getSize().y);
    m_view.setCenter(m_window.getSize().x / 2, m_window.getSize().y / 2);
    m_window_renderer.m_view = m_view;

    m_ui = std::make_unique<UI>(m_window, m_textures, m_slots);

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
    case SDLK_LEFT:
        m_is_moving_left = true;
        break;
    case SDLK_RIGHT:
        m_is_moving_right = true;
        break;
    case SDLK_UP:
        m_is_moving_up = true;
        break;
    case SDLK_DOWN:
        m_is_moving_down = true;
        break;
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
    case SDLK_LEFT:
        m_is_moving_left = false;
        break;
    case SDLK_RIGHT:
        m_is_moving_right = false;
        break;
    case SDLK_UP:
        m_is_moving_up = false;
        break;
    case SDLK_DOWN:
        m_is_moving_down = false;
        break;
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


    Text test_text("brown fox jumps over the lazy dog");
    test_text.setFont(m_test_font.get());
    test_text.setPosition(400, 300);
    test_text.setScale(1, 1);
    test_text.setColor({255, 0,255,255});

    m_window_renderer.getShaders().refresh();

    auto mouse_coords = m_window_renderer.getMouseInWorld();
    Sprite test_sprite(m_test_font->getTexture());//*m_textures.get("arrow"));
    test_sprite.m_color = {0, 0, 0, 255};
    test_sprite.setScale(400, 300);
    test_sprite.setPosition(400, 300);
    m_window.clear({1, 1, 1, 1});
    m_window_renderer.getShader("Text").use();
    m_window_renderer.getShader("Text").setUniform2("u_time", m_time);
    m_window_renderer.drawSprite(test_sprite, "Text", DrawType::Dynamic);
    test_text.setPosition(mouse_coords);
    m_window_renderer.drawText(test_text, "Text", DrawType::Dynamic);
    m_window_renderer.drawAll();

    m_window_renderer.m_view = m_view;

    m_ui->draw(m_window);
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
