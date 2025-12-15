#include "PostEffects.h"
#include "CommonShaders.inl"

#include "Shader.h"

using bf = BlendFactor;

Bloom::Bloom(int width, int height, int gauss_pass_count, float brightness_threshold)
    : m_bloom_pixels1(width, height),
      m_bloom_pixels2(width, height),
      m_brightness_pass(std::string(vertex_sprite_code_direct), std::string(fragment_brightness_code)),
      m_gauss_vert_pass(std::string(vertex_sprite_code_direct), std::string(fragment_gauss_vert_code)),
      m_gauss_horiz_pass(std::string(vertex_sprite_code_direct), std::string(fragment_gauss_horiz_code)),
      m_combine_pass(std::string(vertex_sprite_code_direct), std::string(fragment_combine_bloom_code)),
      m_gauss_pass_count(gauss_pass_count),
      m_brightness_threshold(brightness_threshold)
{
}

void Bloom::process(Texture &source, Renderer &target)
{

    auto old_blend_factors = target.m_blend_factors;

    m_screen_sprite.draw(m_bloom_pixels2, m_brightness_pass, source);

    for (int pass = 0; pass < m_gauss_pass_count; ++pass)
    {
        m_bloom_pixels1.clear({0, 0, 0, 0});
        m_screen_sprite.draw(m_bloom_pixels1, m_gauss_vert_pass, m_bloom_pixels2.getTexture());

        m_bloom_pixels2.clear({0, 0, 0, 0});
        m_screen_sprite.draw(m_bloom_pixels2, m_gauss_horiz_pass, m_bloom_pixels1.getTexture());
    }

    setBlendParams({bf::One, bf::OneMinusSrcAlpha, bf::One, bf::OneMinusSrcAlpha});
    m_screen_sprite.draw(target.getTarget(), m_combine_pass, source, m_bloom_pixels2.getTexture());
}
BloomPhysical::BloomPhysical(int width, int height, int mip_count, int gauss_pass_count, TextureOptions options, std::string final_shader)
    : m_options(options),
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


BloomFinal::BloomFinal(int width, int height, int mip_count, int gauss_pass_count, float brightness_threshold, TextureOptions options)
    : m_brightness_pass(std::string(vertex_sprite_code_direct), std::string(fragment_brightness_code)),
      m_gauss_vert_pass(std::string(vertex_sprite_code_direct), std::string(fragment_gauss_vert_code)),
      m_gauss_horiz_pass(std::string(vertex_sprite_code_direct), std::string(fragment_gauss_horiz_code)),
      m_combine_pass(std::string(vertex_sprite_code_direct), std::string(fragment_combine_bloom_code)),
      m_downsample_pass(std::string(vertex_sprite_code_direct), std::string(fragment_downsample_code)),
      m_full_pass(std::string(vertex_sprite_code_direct), std::string(fragment_fullpass_code)),
      m_gauss_pass_count(gauss_pass_count),
      m_brightness_threshold(brightness_threshold)
{
    initMips(mip_count, width, height, options);
}

void BloomFinal::initMips(int n_levels, int width, int height, TextureOptions options)
{
    m_mips.clear();
    m_mips.reserve(n_levels); //! THIS IS IMPORTANT, OTHERWISE YOU INVALIDATE PREVIOUS MIPS  WHEN PUSHING BACK.
    for (int i = 0; i < n_levels; ++i)
    {
        m_mips.emplace_back(width, height, options);
        width /= 2;
        height /= 2;
    }
}

void BloomFinal::process(Texture &source, Renderer &target)
{
    auto old_factors = target.m_blend_factors;

    //! BRIGHTNESS PASS
    auto &first_mip = m_mips.front();
    first_mip.pixels.clear({0, 0, 0, 0});
    m_screen_sprite.draw(first_mip.pixels, m_brightness_pass, source);

    for (int pass = 0; pass < m_gauss_pass_count; ++pass)
    {
        first_mip.pixels_tmp.clear({0, 0, 0, 0});
        m_screen_sprite.draw(first_mip.pixels_tmp, m_gauss_vert_pass, first_mip.pixels.getTexture());

        first_mip.pixels.clear({0, 0, 0, 0});
        m_screen_sprite.draw(first_mip.pixels, m_gauss_horiz_pass, first_mip.pixels_tmp.getTexture());
    }

    for (size_t mip_id = 1; mip_id < m_mips.size(); ++mip_id)
    {
        auto &prev_mip = m_mips.at(mip_id - 1);
        auto &mip = m_mips.at(mip_id);

        glm::vec2 mip_resolution = glm::vec2(mip.pixels.getSize().x, mip.pixels.getSize().y);
        m_downsample_pass.setUniform("u_src_resolution", mip_resolution);
        m_screen_sprite.draw(mip.pixels, m_downsample_pass, prev_mip.pixels.getTexture());

        for (int pass = 0; pass < m_gauss_pass_count; ++pass)
        {
            mip.pixels_tmp.clear({0, 0, 0, 0});
            m_screen_sprite.draw(mip.pixels_tmp, m_gauss_vert_pass, mip.pixels.getTexture());

            mip.pixels.clear({0, 0, 0, 0});
            m_screen_sprite.draw(mip.pixels, m_gauss_horiz_pass, mip.pixels_tmp.getTexture());
        }

        // ! accumulate results into first mip
        setBlendParams({bf::One, bf::One, bf::One, bf::One});
        m_screen_sprite.draw(first_mip.pixels, m_full_pass, mip.pixels.getTexture());
        setBlendParams({bf::SrcAlpha, bf::OneMinusSrcAlpha, bf::SrcAlpha, bf::OneMinusSrcAlpha});
    }

    auto &mip = m_mips.front();
    setBlendParams({bf::One, bf::OneMinusSrcAlpha, bf::One, bf::OneMinusSrcAlpha});
    m_screen_sprite.draw(target.getTarget(), m_combine_pass, mip.pixels.getTexture());
}
