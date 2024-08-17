#include "ShaderUI.h"

#include <filesystem>
#include <variant>

#include "Window.h"
#include "FrameBuffer.h"
#include "Utils/IO.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../external/stbimage/stb_image_write.h"

#include <magic_enum.hpp>
#include <magic_enum_utility.hpp>

UIWindow::UIWindow(std::string name) : name(name)
{
}

struct ColorByte
{
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;
};

struct Image
{

    Image(int x, int y)
        : x_size(x), y_size(y), pixels(x * y)
    {
    }

    ColorByte *data()
    {
        return pixels.data();
    }

    int x_size;
    int y_size;

    std::vector<ColorByte> pixels;
};

void writeTextureToFile(std::filesystem::path path, std::string filename, FrameBuffer &buffer)
{
    int width = buffer.getSize().x;
    int height = buffer.getSize().y;
    Image image(width, height);
    buffer.bind();
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    auto full_path = (path.string() + filename);
    int check = stbi_write_png(full_path.c_str(), width, height, 4, image.data(), 4 * width);
    if (check == 0)
    {
        std::cout << "ERROR WRITING FILE: " << full_path << "\n";
    }
}

UI::UI(Window &window,TextureHolder &textures,
       std::vector<ShaderSlot> &slots)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window.getHandle(), window.getContext());
    ImGui_ImplOpenGL3_Init();

    auto shaders_window = std::make_unique<ShadersWindow>(textures, slots);

    m_window_data[UIWindowType::SHADERS].p_window = std::move(shaders_window);
    m_window_data[UIWindowType::SHADERS].name = "Shaders";
    m_window_data[UIWindowType::SHADERS].is_active = true;
}

std::vector<std::string> extractFragmentShaderNames(const std::filesystem::path shader_dir = "../Resources/")
{
    std::vector<std::string> shader_names;
    for (auto const &dir_entry : std::filesystem::directory_iterator{shader_dir})
        shader_names.push_back(dir_entry.path());
    return shader_names;
}

ShadersWindow::ShadersWindow(TextureHolder &textures, std::vector<ShaderSlot> &slots)
    : UIWindow("Shaders"), m_slots(slots), m_textures(textures)
{
}

ShadersWindow::~ShadersWindow() {}

template <class T>
using uncvref_t = std::remove_cv_t<std::remove_reference_t<T>>;
template <typename T>
struct fail : std::false_type
{
};

void ShadersWindow::drawUniformValue(const char *name, UniformType &value)
{

    std::visit([&name](auto &value)
               {
                    using T = uncvref_t<decltype(value)>; 
                   if constexpr (std::is_same_v<T, int>)
                    {                       
                       if(ImGui::InputInt(name, &value)){
                            
                       }
                    }
                    else if constexpr (std::is_same_v<T, float>)
                    {
                       if(ImGui::SliderFloat(name, &value, 0., 1.))
                       {
                       }
                     }
                   else if constexpr (std::is_same_v<T, bool>)
                    {   
                        if(ImGui::Button(name))
                        {
                            value != value; 
                        }
                    }
                   else if constexpr (std::is_same_v<T, glm::vec2>)
                   {
                        float* val = &value[0];
                        ImGui::InputFloat2(name, val);
                   }
                   else if constexpr (std::is_same_v<T, glm::vec3>)
                   {
                        float* val = &value[0];
                        ImGui::ColorPicker3(name, val);
                   }
                   else if constexpr (std::is_same_v<T, glm::vec4>)
                   {
                        float* val = &value[0];
                        ImGui::ColorPicker4(name, val);
                   }
                   else 
                   {
                         static_assert(fail<T>{}, "we should not get here!");
                   } },
               value);
}

void ShadersWindow::drawShaderSlot(ShaderSlot &slot)
{
    auto window_size = ImGui::GetWindowSize();
    ImVec2 shader_box_size = {0.45f * window_size.x, window_size.y / 4.2f};
    ImGui::Text("Shaders");
    if (ImGui::BeginListBox("Shaders", shader_box_size))
    {

        for (auto &[id, shader_data] : m_slots.at(0).m_canvas.getShaders().getAllData())
        {
            const bool is_selected = (slot.m_selected_shader == id);
            if (ImGui::Selectable(shader_data.p_program->getName().c_str(), is_selected))
            {
                slot.m_selected_shader = id;
                slot.m_selected_uniform = ""; //! unselect uniform
            }
        }
        ImGui::EndListBox();
    }

    if (slot.m_selected_shader.empty())
    {
        return;
    }
    auto &shader = slot.m_canvas.getShader(slot.m_selected_shader);
    auto &shader_variables = shader.getVariables();
    ImGui::SameLine();
    if (ImGui::BeginListBox("", shader_box_size))
    {
        for (auto &[name, value] : shader_variables.uniforms)
        {
            const bool is_selected = (slot.m_selected_uniform == name);
            if (ImGui::Selectable(name.c_str(), is_selected))
            {
                slot.m_selected_uniform = name;
                shader.saveUniformValue(name, value);
            }
        }
        ImGui::EndListBox();
    }
    for (auto &[texture_name, value] : shader_variables.textures)
    {
        if (ImGui::BeginListBox(texture_name.c_str(), shader_box_size))
        {
            for (auto &[texture_identifier, texture_ptr] : m_textures.m_textures)
            {
                const bool is_selected = (value.handle == texture_ptr->getHandle());
                if (ImGui::Selectable(texture_identifier.c_str(), is_selected))
                {
                    value.handle = texture_ptr->getHandle(); //! set shader texture to selected texture
                    slot.m_selected_texture = texture_identifier;
                }
            }
            ImGui::EndListBox();
        }
    }

    if (slot.m_selected_uniform != "")
    {
        drawUniformValue(slot.m_selected_uniform.c_str(),
                         shader_variables.uniforms.at(slot.m_selected_uniform));
    }
}

void ShadersWindow::refresh()
{
    std::filesystem::path path = "../Resources/";
    auto shader_filenames = extractNamesInDirectory(path, ".frag");

    std::vector<std::string> new_shader_paths;

    auto& default_slot = m_slots.at(0); 
    for (auto &shader_filename : shader_filenames)
    {

        auto pos_right = shader_filename.find_last_of('.');
        std::string shader_name = shader_filename.substr(0, pos_right);
        if (default_slot.m_canvas.getShaders().getAllData().count(shader_name) == 0)
        {
            for (auto &slot : m_slots)
            {
                slot.m_canvas.addShader(shader_name, path.string() + "basicinstanced.vert",
                                        path.string() + shader_filename);
            }
        }

        new_shader_paths.push_back(shader_filename);
    }
}

void ShadersWindow::draw()
{
    {
        std::filesystem::path path = "../Resources/";
        auto names = extractNamesInDirectory(path, ".png");

        ImGui::Begin("Shader Manager");
        auto shader_names = extractNamesInDirectory(path, ".frag");
        if (ImGui::Button("Refresh"))
        {
            refresh();
        }
        ImGui::End();



        ImGui::Begin("Shader Slots");
        int i = 0;
        for (auto &slot : m_slots)
        {
            if (ImGui::TreeNode(("Slot: " + std::to_string(i)).c_str()))
            {
                drawShaderSlot(slot);
                m_output_image_name.resize(50);
                if (ImGui::InputText("File name: ", m_output_image_name.data(), m_output_image_name.size()))
                {
                }
                if (ImGui::Button("Draw Texture"))
                {
                    writeTextureToFile("./", m_output_image_name, slot.m_pixels);
                }
                ImGui::TreePop();
            }

            i++;
        }
        ImGui::End();
    }
}

UIWindow::~UIWindow()
{
}

void UI::showWindow()
{
}

void UI::draw(Window &window)
{

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    bool show_demo_window = true;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    std::vector<UIWindowType> active_windows;
    ImGui::Begin("Control Panel"); // Create a window called "Hello, world!" and append into it.
    for (auto &[type, data] : m_window_data)
    {
        if (ImGui::Button(data.name.c_str()))
            data.is_active = !data.is_active;
    }
    
        ImGui::Begin("Simulation");
        if(ImGui::Button("Start"))
        {
            m_simulation_on = true;
        }
        if(ImGui::Button("Stop"))
        {
            m_simulation_on = false;
        }
        if(ImGui::Button("Reset"))
        {
            m_simulation_on = false;
            m_reset = true;
        }
        if(ImGui::InputInt("Simulation slot", &m_simulation_slot))
        {
            if(m_simulation_slot > 4)
            {
                m_simulation_slot = 0;
            }
        }
        
        ImGui::InputFloat4("Particle Init Color", &m_particle_init_color.r);
        ImGui::InputFloat4("Particle End Color", &m_particle_end_color.r);

        ImGui::End();



    ImGui::End();

    for (auto &[type, window_data] : m_window_data)
    {
        if (window_data.is_active)
        {
            window_data.p_window->draw();
        }
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::handleEvent(SDL_Event event)
{
    ImGui_ImplSDL2_ProcessEvent(&event);
}