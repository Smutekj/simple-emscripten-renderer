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

    int n_slots_x = 2;
    int n_slots_y = 2;
    m_slots.reserve(n_slots_x * n_slots_y);
    // for (int i = 0; i < n_slots_x * n_slots_y; ++i)
    // {
    //     m_slots.emplace_back(width / n_slots_x, height / n_slots_y);
    // }

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

    m_particles = std::make_unique<Particles>(2000);
    m_particles->setLifetime(2.f);
    m_particles->setUpdater([this](Particle &p)
                            {
                                auto t_left = p.life_time - p.time;
                                p.acc = {0, 0};
                                p.vel += p.time *p.acc;
                                p.pos += p.vel * 0.016f;
                                p.scale += utils::Vector2f{0.5f};
                                p.angle += randf(0, 3.); });
    m_particles->setEmitter([](utils::Vector2f spawn_pos)
                            {
                                Particle p;
                                p.pos = spawn_pos + utils::Vector2f{randf(-50,50), randf(0, 10.f)};
                                p.vel = {30+randf(-20, 20), randf(40, 50)};
                                p.scale = {10.2, 10.2};
                                return p; });
    m_particles->setRepeat(true);
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

    // auto &shader_slot = m_slots.at(0);
    // auto slot_size = shader_slot.getSize();
    // int slot_ind = 0;
    // for (auto &shader_slot : m_slots)
    // {
    //     // if (slot_ind == m_ui->getSimulationSlot())
    //     // {
    //     // slot_ind++;
    //     // continue;
    //     // }
    //     auto slot_size = shader_slot.getSize();

    //     Sprite test_sprite(shader_slot.m_pixels.getTexture());
    //     test_sprite.setScale(slot_size.x / 2.f, slot_size.y / 2.f);
    //     test_sprite.setPosition(slot_size.x / 2.f, slot_size.y / 2.f);
    //     if (!shader_slot.m_selected_shader.empty())
    //     {
    //         auto &shader = shader_slot.m_canvas.getShader(shader_slot.m_selected_shader);
    //         shader.setUniform2("u_time", m_time);
    //         for (auto &[texture_name, texture_data] : shader.getVariables().textures)
    //         {
    //             test_sprite.m_texture_handles.at(texture_data.slot) = texture_data.handle;
    //         }
    //         shader_slot.m_canvas.m_view.setCenter(test_sprite.getPosition().x, test_sprite.getPosition().y);
    //         shader_slot.m_canvas.m_view.setSize(slot_size.x, slot_size.y);
    //         shader_slot.draw(test_sprite);
    //     }
    //     slot_ind++;
    // }

    // m_swirl_renderer1.clear({0, 0, 0, 1});
    // m_swirl_renderer1.m_view = m_view;
    // m_particles->setSpawnPos({mouse_coords.x, mouse_coords.y});
    // m_particles->update(0.01f);
    // m_particles->setInitColor(m_ui->getParticleInitColor());
    // m_particles->setFinalColor(m_ui->getParticleEndColor());
    // m_particles->draw(m_swirl_renderer1);

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

    // m_window_renderer.drawAll();
    // doBloom(b1.getTexture(), m_window_renderer);

    // int row = 0;
    // int col = 0;
    // float left_margin = 0;
    // float top_margin = 0;

    // for (auto &shader_slot : m_slots)
    // {
    //     auto slot_size = shader_slot.getSize();

    //     Sprite screen_sprite(shader_slot.m_pixels.getTexture());
    //     screen_sprite.setScale(slot_size.x / 2.f, slot_size.y / 2.f);
    //     screen_sprite.setPosition(left_margin + slot_size.x / 2.f, top_margin + slot_size.y / 2.f);
    //     // m_window_renderer.drawSprite(screen_sprite, "Instanced", GL_DYNAMIC_DRAW);

    //     left_margin += slot_size.x;
    //     row++;
    //     if (row == 2)
    //     {
    //         row = 0;
    //         col++;
    //         left_margin = 0;
    //         top_margin += slot_size.y;
    //     }
    // }
    m_window_renderer.m_view = m_view;
    // m_window_renderer.drawAll();

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
