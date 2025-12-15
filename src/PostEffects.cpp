#include "PostEffects.h"
#include "CommonShaders.inl"

#include "Shader.h"

using bf = BlendFactor;

Bloom::Bloom(int width, int height)
    : m_bloom_pass1(width, height),
      m_bloom_pass2(width, height),
      m_bloom_renderer1(m_bloom_pass1),
      m_bloom_renderer2(m_bloom_pass2)
{
    m_bloom_renderer1.getShaders().loadFromCode("gaussVert", vertex_sprite_code, fragment_gauss_vert_code);
    m_bloom_renderer2.getShaders().loadFromCode("gaussHoriz", vertex_sprite_code, fragment_gauss_horiz_code);
    m_bloom_renderer2.getShaders().loadFromCode("brightness", vertex_sprite_code, fragment_brightness_code);

    m_bloom_renderer1.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
    m_bloom_renderer2.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
}

void Bloom::process(Texture &source, Renderer &target)
{
    if (!target.hasShader("combineBloom"))
    {
        target.getShaders().loadFromCode("combineBloom", vertex_sprite_code, fragment_combine_bloom_code);
    }

    auto old_view = target.m_view;
    auto old_blend_factors = target.m_blend_factors;

    auto target_size = target.getTargetSize();
    Sprite screen_sprite(source);
    screen_sprite.setPosition(target_size / 2.f);
    screen_sprite.setScale(target_size / 2.f);

    m_bloom_renderer1.m_view.setCenter(screen_sprite.getPosition());
    m_bloom_renderer1.m_view.setSize(target_size);

    m_bloom_renderer2.m_view.setCenter(screen_sprite.getPosition());
    m_bloom_renderer2.m_view.setSize(target_size);

    m_bloom_renderer1.clear({0, 0, 0, 0});
    m_bloom_renderer2.clear({0, 0, 0, 0});

    m_bloom_renderer2.drawSprite(screen_sprite, "brightness");
    m_bloom_renderer2.drawAll();

    for (int pass = 0; pass < 4; ++pass)
    {
        m_bloom_renderer1.clear({0, 0, 0, 0});
        screen_sprite.setTexture(m_bloom_pass2.getTexture());
        m_bloom_renderer1.drawSprite(screen_sprite, "gaussVert");
        m_bloom_renderer1.drawAll();

        m_bloom_renderer2.clear({0, 0, 0, 0});
        screen_sprite.setTexture(m_bloom_pass1.getTexture());
        m_bloom_renderer2.drawSprite(screen_sprite, "gaussHoriz");
        m_bloom_renderer2.drawAll();
    }

    screen_sprite.setTexture(0, source);
    screen_sprite.setTexture(1, m_bloom_pass2.getTexture());
    auto pixels_size = m_bloom_pass2.getSize();
    screen_sprite.setPosition(pixels_size / 2.f);
    screen_sprite.setScale(pixels_size / 2.f);
    target.m_view.setCenter(pixels_size / 2.f);
    target.m_view.setSize(pixels_size);

    target.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::OneMinusSrcAlpha};
    target.drawSprite(screen_sprite, "combineBloom");
    target.drawAll();

    target.m_view = old_view;
    target.m_blend_factors = old_blend_factors;
}

BloomFinal::BloomFinal(int width, int height, int mip_count, int gauss_pass_count, TextureOptions options, std::string final_shader)
    : m_gauss_pass_count(gauss_pass_count), m_final_shader(final_shader)
{
    if (m_final_shader.empty())
    {
        m_final_shader = "combineBloom";
    }
    initMips(mip_count, width, height, options);
}

void BloomFinal::initMips(int n_levels, int width, int height, TextureOptions options)
{
    m_mips.clear();
    m_mips.reserve(n_levels); //! THIS IS IMPORTANT, OTHERWISE YOU INVALIDATE PREVIOUS MIPS  WHEN PUSHING BACK.
    for (int i = 0; i < n_levels; ++i)
    {
        m_mips.emplace_back(width, height, options);
        m_mips.at(i).canvas.getShaders().loadFromCode("downsample", vertex_sprite_code, fragment_downsample13_code);
        m_mips.at(i).canvas_tmp.getShaders().loadFromCode("gaussVert", vertex_sprite_code, fragment_gauss_vert_code);
        m_mips.at(i).canvas.getShaders().loadFromCode("gaussHoriz", vertex_sprite_code, fragment_gauss_horiz_code);
        width /= 2;
        height /= 2;
    }

    //! only the first mip does brightness pass
    m_mips.front().canvas.getShaders().loadFromCode("brightness", vertex_sprite_code, fragment_brightness_code);
    m_mips.front().canvas.getShader("brightness").setUniform("u_brightness_threshold", 0.0f);
}

void BloomFinal::process(Texture &source, Renderer &target)
{
    if (!target.hasShader(m_final_shader))
    {
        std::cout << "Selected final shader: " + m_final_shader + " does not exist! Will Use default CombineBloom instead" << std::endl;
        ;
        target.getShaders().loadFromCode(m_final_shader, vertex_sprite_code, fragment_combine_bloom_code);
    }

    for (auto &mip : m_mips)
    {
        mip.canvas.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
        mip.canvas_tmp.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
    }
    auto old_view = target.m_view;
    auto old_factors = target.m_blend_factors;

    utils::Vector2f target_size = {target.getTargetSize().x, target.getTargetSize().y};
    Sprite screen_sprite(source);
    screen_sprite.setPosition(target_size / 2.f);
    screen_sprite.setScale(target_size / 2.f);
    auto size = target_size;

    //! BRIGHTNESS PASS
    screen_sprite.setTexture(source);
    auto &first_mip = m_mips.front();
    first_mip.canvas.clear({0, 0, 0, 0});
    first_mip.canvas.m_view.setCenter(screen_sprite.getPosition());
    first_mip.canvas.m_view.setSize(screen_sprite.getScale() * 2.f);
    first_mip.canvas.drawSprite(screen_sprite, "brightness");
    first_mip.canvas.drawAll();

    for (int pass = 0; pass < m_gauss_pass_count; ++pass)
    {
        utils::Vector2f mip_size = {first_mip.pixels.getSize().x, first_mip.pixels.getSize().y};
        // Sprite ss1(first_mip.pixels.getTexture());
        screen_sprite.setPosition(mip_size / 2.f);
        screen_sprite.setScale(mip_size / 2.f);
        first_mip.canvas_tmp.clear({0, 0, 0, 0});
        screen_sprite.setTexture(first_mip.pixels.getTexture());
        first_mip.canvas_tmp.m_view.setCenter(screen_sprite.getPosition());
        first_mip.canvas_tmp.m_view.setSize(screen_sprite.getScale() * 2.);
        first_mip.canvas_tmp.drawSprite(screen_sprite, "gaussVert");
        first_mip.canvas_tmp.drawAll();

        // Sprite ss2(first_mip.pixels_tmp.getTexture());
        first_mip.canvas.clear({0, 0, 0, 0});
        screen_sprite.setTexture(first_mip.pixels_tmp.getTexture());
        first_mip.canvas.m_view.setCenter(screen_sprite.getPosition());
        first_mip.canvas.m_view.setSize(screen_sprite.getScale() * 2.);
        first_mip.canvas.drawSprite(screen_sprite, "gaussHoriz");
        first_mip.canvas.drawAll();
    }

    size = target_size / 2;
    for (size_t mip_id = 1; mip_id < m_mips.size(); ++mip_id)
    {
        auto &prev_mip = m_mips.at(mip_id - 1);
        auto &mip = m_mips.at(mip_id);
        mip.canvas.m_view.setCenter(screen_sprite.getPosition());
        mip.canvas.m_view.setSize(target_size);

        screen_sprite.setTexture(prev_mip.pixels.getTexture());
        screen_sprite.setPosition(screen_sprite.getPosition());
        screen_sprite.setScale(size / 2.f);
        mip.canvas.m_view.setCenter(screen_sprite.getPosition());
        mip.canvas.m_view.setSize(screen_sprite.getScale() * 2.);
        mip.canvas.clear({0, 0, 0, 0});
        glm::vec2 mip_resolution = glm::vec2(mip.canvas.getTargetSize().x, mip.canvas.getTargetSize().y);
        mip.canvas.getShader("downsample").setUniform("u_src_resolution", mip_resolution);
        mip.canvas.drawSprite(screen_sprite, "downsample");
        mip.canvas.drawAll();

        for (int pass = 0; pass < m_gauss_pass_count; ++pass)
        {
            mip.canvas_tmp.clear({0, 0, 0, 0});
            screen_sprite.setTexture(mip.pixels.getTexture());
            mip.canvas_tmp.m_view.setCenter(screen_sprite.getPosition());
            mip.canvas_tmp.m_view.setSize(screen_sprite.getScale() * 2.);
            mip.canvas_tmp.drawSprite(screen_sprite, "gaussVert");
            mip.canvas_tmp.drawAll();

            mip.canvas.clear({0, 0, 0, 0});
            screen_sprite.setTexture(mip.pixels_tmp.getTexture());

            mip.canvas.drawSprite(screen_sprite, "gaussHoriz");
            mip.canvas.drawAll();
        }

        // ! accumulate results into first mip
        first_mip.canvas.m_blend_factors = {bf::One, bf::One, bf::One, bf::One};
        screen_sprite.setTexture(mip.pixels.getTexture());
        utils::Vector2f mip_size = first_mip.canvas.getTargetSize(); // mip.pixels.getSize();
        screen_sprite.setPosition(mip_size / 2.f);
        screen_sprite.setScale(mip_size / 2.f);
        first_mip.canvas.drawSprite(screen_sprite);
        first_mip.canvas.drawAll();

        size = size / 2;
    }

    int i = 0;
    {
        // target.m_blend_factors = {bf::One, bf::One, bf::One, bf::OneMinusSrcAlpha};

        auto &mip = m_mips.front();
        // screen_sprite.setTexture(0, source); //! DO NOT CHANGE ORDER OF SETTEXTURES!!
        screen_sprite.setTexture(0, mip.pixels.getTexture());
        auto pixels_size = target_size; // mip.pixels.getSize();
        screen_sprite.setPosition(pixels_size.x / 2.f, pixels_size.y / 2.f);
        screen_sprite.setScale(pixels_size.x / 2.f, pixels_size.y / 2.f);
        target.m_view.setCenter(pixels_size.x / 2.f, pixels_size.y / 2.f);
        target.m_view.setSize(pixels_size.x, pixels_size.y);
        target.drawSprite(screen_sprite, m_final_shader);
        target.drawAll();

        i++;
    }

    target.m_blend_factors = old_factors;
    target.m_view = old_view;
}

BloomPhysical::BloomPhysical(int width, int height, int mip_count, int gauss_pass_count, TextureOptions options, std::string final_shader)
    : 
    m_options(options),
      m_bright_pixels(width, height, options),
      m_brightness_pass(std::string{vertex_sprite_code_direct}, std::string{fragment_brightness_code}),
      m_downsample_pass(std::string{vertex_sprite_code_direct}, std::string{fragment_downsample13_code}),
      m_upsample_pass(std::string{vertex_sprite_code_direct}, std::string{fragment_upsample_blur_code}),
      m_mixer_pass(std::string{vertex_sprite_code_direct}, std::string{fragment_upsample_mix_code})
{
    initMips(mip_count, width, height, options);
    m_upsample_pass.setUniform("u_filter_radius", 0.001f);
    m_brightness_pass.setUniform("u_threshold", 0.f);
}

float BloomPhysical::TexMip::getAspect() const
{
    return getWidth() / getHeight();
}
float BloomPhysical::TexMip::getWidth() const
{
    return pixels.getSize().x;
}
float BloomPhysical::TexMip::getHeight() const
{
    return pixels.getSize().y;
}

BloomPhysical::TexMip::TexMip(int width, int height, TextureOptions option)
    : pixels(width, height, option)
{
}

void BloomPhysical::initMips(int n_levels, int width, int height, TextureOptions options)
{
    m_mips.clear();
    m_mips.reserve(n_levels); //! THIS IS IMPORTANT, OTHERWISE YOU INVALIDATE PREVIOUS MIPS  WHEN PUSHING BACK.
    for (int i = 0; i < n_levels; ++i)
    {
        width /= 2;
        height /= 2;
        m_mips.emplace_back(width, height, options);
    }
}

void BloomPhysical::process(Texture &source, Renderer &target)
{

    // mip.canvas.m_blend_factors = {bf::One, bf::One, bf::One, bf::Zero};
    auto old_view = target.m_view;
    auto old_factors = target.m_blend_factors;

    m_bright_pixels.clear({0, 0, 0, 0});
    m_screen_sprite.draw(m_bright_pixels, m_brightness_pass, source);
    // writeTextureToFile("../", "Bright.png", m_bright_pixels.getTexture());

    //! create downsampled textures
    auto *prev_texture = &m_bright_pixels.getTexture();
    int i = 0;
    for (auto &mip : m_mips)
    {
        mip.pixels.clear({0, 0, 0, 0});
        m_downsample_pass.setUniform("u_src_resolution", glm::vec2(mip.getWidth(), mip.getHeight()));
        m_screen_sprite.draw(mip.pixels, m_downsample_pass, *prev_texture);
        // writeTextureToFile("../", "DS-mip-" + std::to_string(i) + ".png", mip.pixels);
        prev_texture = &mip.pixels.getTexture();
        i++;
    }

    for (int mip_id = m_mips.size() - 2; mip_id >= 0; mip_id--)
    {
        prev_texture = &m_mips.at(mip_id + 1).pixels.getTexture();
        auto &mip = m_mips.at(mip_id);
        mip.pixels.clear({0, 0, 0, 0});
        m_upsample_pass.setUniform("u_aspect_ratio", mip.getAspect());
        m_screen_sprite.draw(mip.pixels, m_upsample_pass, *prev_texture);
    }
    // writeTextureToFile("../", "US-mip-" + std::to_string(1) + ".png", m_mips.at(0).pixels);

    target.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::OneMinusSrcAlpha};
    auto &mip = m_mips.front();
    m_screen_sprite.draw(target.getTarget(), m_mixer_pass, source, mip.pixels.getTexture());

    target.m_blend_factors = old_factors;
    target.m_view = old_view;
}


LightCombine::LightCombine(int width, int height)
    : m_multiply_texture(width, height),
    m_multiply_pass(std::string(vertex_sprite_code_direct), std::string(fragment_combine_light))
{
}

void LightCombine::process(Texture &source, Renderer &target)
{
    setBlendParams({bf::Zero, bf::SrcColor, bf::One, bf::Zero}); 
    m_screen_sprite.draw(target.getTarget(), m_multiply_pass, source);
}

BloomSmoke::BloomSmoke(int width, int height)
    : m_bloom_pass1(width, height),
      m_bloom_pass2(width, height),
      m_downsampled_pixels3(width / 8, height / 8),
      m_downsampled_pixels33(width / 8, height / 8),
      m_bloom_renderer1(m_bloom_pass1),
      m_bloom_renderer2(m_bloom_pass2),
      m_downsampler3(m_downsampled_pixels3),
      m_downsampler33(m_downsampled_pixels33)
{

    m_downsampler3.addShader("gaussHoriz", "basicinstanced.vert", "gaussHoriz.frag");
    m_downsampler3.addShader("brightness", "basicinstanced.vert", "brightness.frag");
    m_downsampler33.addShader("gaussVert", "basicinstanced.vert", "gaussVert.frag");

    m_downsampler3.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
    m_downsampler33.m_blend_factors = {bf::One, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
}

void BloomSmoke::process(Texture &source, Renderer &target)
{

    if (!target.hasShader("combineSmoke"))
    {
        target.addShader("combineSmoke", "basicinstanced.vert", "combineSmoke.frag");
    }

    auto old_view = target.m_view;
    auto old_blend_factors = target.m_blend_factors;

    auto target_size = target.getTargetSize();
    Sprite screen_sprite(source);
    screen_sprite.setPosition(target_size.x / 2.f, target_size.y / 2.f);
    screen_sprite.setScale(target_size.x / 2.f, target_size.y / 2.f);

    m_downsampler3.m_view.setCenter(screen_sprite.getPosition().x, screen_sprite.getPosition().y);
    m_downsampler3.m_view.setSize(target_size.x, target_size.y);

    //! BRIGHTNESS PASS
    screen_sprite.setTexture(source);
    m_downsampler3.clear({0, 0, 0, 0});
    m_downsampler3.drawSprite(screen_sprite, "brightness");
    m_downsampler3.drawAll();

    auto size = m_downsampled_pixels3.getSize();
    screen_sprite.setTexture(m_downsampled_pixels3.getTexture());
    screen_sprite.setPosition(size / 2.f);
    screen_sprite.setScale(size / 2.f);
    m_downsampler33.m_view.setCenter(size / 2.f);
    m_downsampler33.m_view.setSize(size);
    m_downsampler33.clear({0, 0, 0, 0});
    m_downsampler33.drawSprite(screen_sprite, "gaussVert");
    m_downsampler33.drawAll();
    // writeTextureToFile("../", "testfile33.png", m_downsampled_pixels33);
    screen_sprite.setTexture(m_downsampled_pixels33.getTexture());
    m_downsampler3.m_view.setCenter(size / 2.f);
    m_downsampler3.m_view.setSize(size);
    screen_sprite.setPosition(size / 2);
    screen_sprite.setScale(size / 2);
    m_downsampler3.drawSprite(screen_sprite, "gaussHoriz");
    m_downsampler3.clear({0, 0, 0, 0});
    m_downsampler3.drawAll();
    // writeTextureToFile("../", "testfile3.png", m_downsampled_pixels3);

    screen_sprite.setTexture(0, source);
    screen_sprite.setTexture(1, m_downsampled_pixels3.getTexture());
    auto pixels_size = m_downsampled_pixels3.getSize();
    screen_sprite.setPosition(pixels_size / 2.f);
    screen_sprite.setScale(pixels_size / 2.f);
    target.m_view.setCenter(pixels_size / 2.f);
    target.m_view.setSize(pixels_size);
    target.drawSprite(screen_sprite, "combineSmoke");

    target.m_blend_factors = {bf::SrcAlpha, bf::OneMinusSrcAlpha};
    target.drawAll();

    target.m_blend_factors = old_blend_factors;
    target.m_view = old_view;
}

EdgeDetect::EdgeDetect(int width, int height)
    : m_vert_pixels(width / 2, height / 2),
      m_horiz_pixels(width / 2, height / 2),
      m_horiz_pass(std::string(vertex_sprite_code_direct), std::string(fragment_gauss_horiz_code)),
      m_vert_pass(std::string(vertex_sprite_code_direct), std::string(fragment_gauss_horiz_code)),
      m_edge_detect_pass(std::string(vertex_sprite_code_direct), std::string(fragment_edge_detect_code)),
      m_combine_edges_pass(std::string(vertex_sprite_code_direct), std::string(fragment_edge_combine_code))
{
    // m_vert_canvas.m_blend_factors = {bf::SrcAlpha, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
    // m_horiz_canvas.m_blend_factors = {bf::SrcAlpha, bf::OneMinusSrcAlpha, bf::One, bf::Zero};
}
void EdgeDetect::process(Texture &source, Renderer &target)
{
    auto old_blend_factors = target.m_blend_factors;

    for (int i = 0; i < 1; ++i)
    {
        m_vert_pixels.clear({0, 0, 0, 0});
        m_screen_sprite.draw(m_vert_pixels, m_vert_pass, source);  

        m_horiz_pixels.clear({0, 0, 0, 0});
        m_screen_sprite.draw(m_horiz_pixels, m_horiz_pass, m_vert_pixels.getTexture());  
    }
    writeTextureToFile("../", "afterGauss", m_horiz_pixels.getTexture());

    m_vert_pixels.clear({0, 0, 0, 0});
    m_screen_sprite.draw(m_vert_pixels, m_edge_detect_pass, m_horiz_pixels.getTexture());  

    writeTextureToFile("../", "edges", m_vert_pixels.getTexture());

    target.m_blend_factors = {bf::One, bf::Zero};
    m_screen_sprite.draw(target.getTarget(), m_combine_edges_pass, source, m_vert_pixels.getTexture());
    target.drawAll();
    target.m_blend_factors = old_blend_factors;
}
