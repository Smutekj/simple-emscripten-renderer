
#pragma once

#include "Vector2.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "Renderer.h"
#include "FrameBuffer.h"
#include "DrawLayer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

enum class UIWindowType
{
    DEBUG = 0,
    SHADERS,
    COUNT
};

//! \struct ShaderUIData
//! \brief ???
struct ShaderUIData
{

    ShaderUIData(Shader &program);

    Shader *p_program = nullptr;
    std::string filename = "";
    VariablesData &variables;
    std::filesystem::file_time_type last_write_time;
};


enum class Multiplier
{
    SCATTER,
    ALIGN,
    SEEK,
    MAX_VEL,
    SLOW_DOWN,
    REPULSE,
    AVOID
};

struct ShaderSlot
{

    ShaderSlot(int width, int height)
        : m_pixels(width, height), m_canvas(m_pixels)
    {
        TextureOptions internal_format;
        internal_format.wrap_x =  TexWrapParam::ClampEdge;
        internal_format.wrap_y =  TexWrapParam::ClampEdge;
        m_layer = std::make_unique<DrawLayer>(width, height, internal_format);
        m_layer->m_canvas.addShader("Instanced", "basicinstanced.vert", "texture.frag");
        m_layer->addEffect(std::make_unique<Bloom3>(width, height));
    }

    utils::Vector2i getSize()
    {
        return m_pixels.getSize();
    }

    void draw(Sprite &test_sprite);

    GLuint getTextureHandle()
    {
        return m_pixels.getHandle();
    }

    void setTexture(int slot, GLuint handle)
    {
    }

    Texture& getPixels()
    {
        return m_layer->m_pixels.getTexture();
    }

public:
    std::unique_ptr<DrawLayer> m_layer;

    FrameBuffer m_pixels;
    Renderer m_canvas;

    std::string m_selected_uniform = "";
    std::string m_selected_texture = "";
    std::string m_selected_shader = "";

    Color m_background_color;
};

constexpr int N_UI_WINDOWS = static_cast<int>(UIWindowType::COUNT);

class UIWindow
{

protected:
    std::string name;
    bool is_active = false;
    std::vector<std::unique_ptr<UIWindow>> children;

public:
    virtual void draw() = 0;
    virtual ~UIWindow() = 0;

    UIWindow(std::string name);

    const std::string &getName() const
    {
        return name;
    }
};

class Shader;

struct ColorData
{
    std::string uniform_name;
    glm::vec4 value = glm::vec4(0.5, 0.1, 0.5, 1.0);
};

struct UniformData
{
    std::string uniform_name;
    UniformType value;
};

struct BuildingLayer;

enum class TextureID
{
    ShaderOut1,
    ShaderOut2,
    Image,
};

class ShadersWindow : public UIWindow
{
    enum Data
    {
        COLOR1,
        COLOR2,
    };

public:
    ShadersWindow(TextureHolder &textures, std::vector<ShaderSlot> &slots);

    virtual ~ShadersWindow();
    virtual void draw() override;

private:
    void drawUniformValue(const char *uniform_nam, UniformType &value);
    void drawShaderSlot(ShaderSlot &slot);
    void refresh();

private:
    TextureHolder &m_textures;

    std::string m_selected_field = "";
    std::string m_output_image_name = "";

    std::vector<ShaderSlot> &m_slots;
};

class FrameBuffer;

class UI
{

    struct UIWindowData
    {
        std::unique_ptr<UIWindow> p_window;
        bool is_active = false;
        std::string name;
    };

    friend UIWindowType;

public:
    UI(Window &window, TextureHolder &textures, std::vector<ShaderSlot> &slots);

    void showWindow();
    void draw(Window &window);
    void handleEvent(SDL_Event event);
    bool simulationRunning() const
    {
        return m_simulation_on;
    }

    bool resetSimulation()
    {
        if (m_reset)
        {
            m_reset = false;
            return true;
        }
        return false;
    }

    int getSimulationSlot()
    {
        return m_simulation_slot;
    }

    Color getParticleInitColor()const{
        return m_particle_init_color;
    }
    Color getParticleEndColor()const{
        return m_particle_end_color;
    }

    int getNParticles()
    {
        return m_draw_particles;
    }

    public:

    std::unordered_map<Multiplier, float> m_force_field;
    utils::Vector2f m_box_size = {500, 500};
    bool m_simulation_on = true;

private:
    int m_draw_particles = 500;
    int m_simulation_slot = 0;
    bool m_reset = false;
    float value;
    utils::Vector2f m_mouse_coords_on_click;
    Color m_particle_init_color = {1,0,0,1};
    Color m_particle_end_color = {1,0,0,1};
    std::unordered_map<UIWindowType, UIWindowData> m_window_data;
};


