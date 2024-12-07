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
            if (tex_coord_x >= 0.5)
            {
                expected_image.pixels.at(index) = {255, 0, 0, 255};
            }
        }
    }

    EXPECT_TRUE(result_image == expected_image);
}

const char *fragment_with_uniforms = "#version 300 es\n"
                                     "precision mediump float;\n"
                                     "uniform float u_test_uniform_float;\n"
                                     "uniform int u_test_uniform_int = 1;\n"
                                     "uniform vec2 u_test_uniform_vec2 = vec2(1.,2.); \n"
                                     "uniform vec3 u_test_uniform_vec3 = vec3(1.,2., 3.); \n"
                                     "uniform vec4 u_test_uniform_vec4 = vec4(1.,2., 3., 4.);\n"
                                     "uniform sampler2D u_test_texture;\n"
                                     "in vec2 v_tex_coord;\n"
                                     "in vec4 v_color;\n"
                                     "out vec4 FragColor;\n"
                                     "void main()\n"
                                     "{\n"
                                     "    float test_float =  u_test_uniform_float;\n"
                                     "    int test_int =  u_test_uniform_int;\n"
                                     "    vec2 test_vec2 =  u_test_uniform_vec2;\n"
                                     "    vec3 test_vec3 =  u_test_uniform_vec3;\n"
                                     "    vec4 test_vec4 =  u_test_uniform_vec4;\n"
                                     "     vec4 test_tex_color = texture(u_test_texture, v_tex_coord);\n"
                                     "    FragColor =   vec4(step(0.5, v_tex_coord.x),0,0,1);\n"
                                     "}";

TEST(TestShaders, UniformRead)
{
    int width = 800;
    int height = 600;
    Window window(width, height);

    Renderer canvas(window);

    canvas.getShaders().loadFromCode("TestShader", (std::string)vertex_font_code, (std::string)fragment_with_uniforms);
    canvas.clear({0, 0, 0, 0});

    auto &shader = canvas.getShaders().get("TestShader");
    auto &variables = shader.getVariables();

    EXPECT_EQ(variables.textures.count("u_test_texture"), 1);

    EXPECT_EQ(variables.uniforms.size(), 5);
    EXPECT_EQ(variables.uniforms.count("u_test_uniform_float"), 1);
    EXPECT_EQ(variables.uniforms.count("u_test_uniform_int"), 1);
    EXPECT_EQ(variables.uniforms.count("u_test_uniform_vec2"), 1);
    EXPECT_EQ(variables.uniforms.count("u_test_uniform_vec3"), 1);
    EXPECT_EQ(variables.uniforms.count("u_test_uniform_vec4"), 1);

    EXPECT_FLOAT_EQ(std::get<float>(variables.uniforms.at("u_test_uniform_float")), 0.f); //! 0 is the default value
    EXPECT_EQ(std::get<int>(variables.uniforms.at("u_test_uniform_int")), 1);

    try
    {
        auto vec2 = std::get<glm::vec2>(variables.uniforms.at("u_test_uniform_vec2"));
        EXPECT_FLOAT_EQ(vec2.x, 1.);
        EXPECT_FLOAT_EQ(vec2.y, 2.);
    }
    catch (std::exception &e)
    {
        std::cout << "error in test uniform vec2: " << e.what() << "\n";
    }

    try
    {
        auto vec3 = std::get<glm::vec3>(variables.uniforms.at("u_test_uniform_vec3"));
        EXPECT_FLOAT_EQ(vec3.x, 1.);
        EXPECT_FLOAT_EQ(vec3.y, 2.);
        EXPECT_FLOAT_EQ(vec3.z, 3.);
    }
    catch (std::exception &e)
    {
        std::cout << "error in test uniform vec3: " << e.what() << "\n";
    }
    try
    {
        auto vec4 = std::get<glm::vec4>(variables.uniforms.at("u_test_uniform_vec4"));
        EXPECT_FLOAT_EQ(vec4.x, 1.);
        EXPECT_FLOAT_EQ(vec4.y, 2.);
        EXPECT_FLOAT_EQ(vec4.z, 3.);
        EXPECT_FLOAT_EQ(vec4.w, 4.);
    }
    catch (std::exception &e)
    {
        std::cout << "error in test uniform vec4: " << e.what() << "\n";
    }
}

// TEST(TestShaders, ShaderHolderLoad)
// {
//     int width = 800;
//     int height = 600;
//     Window window(width, height);

//     FrameBuffer target_pixels(11, 10);
//     Renderer canvas(target_pixels);

//     //! test load from code
//     bool success = canvas.getShaders().loadFromCode("TestShaderFromCode", (std::string)vertex_font_code, (std::string)fragment_font_code);
//     EXPECT_TRUE(success);

//     //! test load from file
//     success = canvas.getShaders().load("TestShaderFromFile", "../Resources/Shaders/basicinstanced.vert",
//                                        "../Resources/Shaders/texture.frag");
//     EXPECT_TRUE(success);
// }