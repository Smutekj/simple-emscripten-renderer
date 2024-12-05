#include <gtest/gtest.h>

#include <Shader.h>
#include <Renderer.h>
#include "../CommonShaders.inl"

TEST(TestShaders, ShaderCompilation)
{
    int width = 800;
    int height = 600;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

    // Create SDL window
    SDL_Window *m_handle =
        SDL_CreateWindow("Test",
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         width, height,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    auto m_gl_context = SDL_GL_CreateContext(m_handle);

    int version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);

    Shader basic_instanced_shader((std::string)vertex_font_code, (std::string)fragment_fullpass_code);

    basic_instanced_shader.use();
}

const char *fragment_fullpass_codetest = "#version 300 es\n"
                                         "precision mediump float;\n"
                                         "in vec2 v_tex_coord;\n"
                                         "in vec4 v_color;\n"
                                         "out vec4 FragColor;\n"
                                         "void main()\n"
                                         "{\n"
                                         "    FragColor =   vec4(step(0.5, v_tex_coord.x),0,0,1);\n"
                                         "}";

TEST(TestShaders, BasicShaderSprite)
{
    int width = 800;
    int height = 600;
    Window window(width, height);

    FrameBuffer target_pixels(11, 10);
    Renderer canvas(target_pixels);

    canvas.getShaders().loadFromCode("TestShader", (std::string)vertex_font_code, (std::string)fragment_fullpass_codetest);
    canvas.clear({0, 0, 0, 0});

    View view;
    view.setCenter(utils::Vector2f{width / 2.f, height / 2.f});
    view.setSize(utils::Vector2f{width, height});
    canvas.m_view = view;

    Sprite screen_sprite;
    screen_sprite.setPosition(view.getCenter());
    screen_sprite.setScale(view.getSize() / 2.f);

    canvas.clear({0, 0, 0, 0});
    canvas.drawSprite(screen_sprite, "TestShader");
    canvas.drawAll();
    Image result_image(target_pixels); //! load data from framebuffer to cpu

    Image expected_image(target_pixels.getSize().x, target_pixels.getSize().y); //! write image by hand
    float pixel_size = 1.f / target_pixels.getSize().x;
    for (int iy = 0; iy < result_image.y_size; ++iy)
    {
        for (int ix = 0; ix < result_image.x_size; ++ix)
        {
            int index = ix + iy * result_image.x_size;
            float tex_coord_x = (float)ix / result_image.x_size + 0.5f * pixel_size;
            expected_image.pixels.at(index) = {0, 0, 0, 255};
            if(tex_coord_x >= 0.5)
            {
                expected_image.pixels.at(index) = {255, 0, 0, 255};
            } 
        }
    }

    
    EXPECT_TRUE(result_image == expected_image);     
}