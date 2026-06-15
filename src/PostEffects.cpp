#include "PostEffects.h"

#include "CommonShaders.inl"
#include "Shader.h"

using bf = BlendFactor;

void PostEffect::resize(int w, int h)
{
    if (w == 0 || h == 0)
    {
        return;
    }
    resizeImpl(w, h);
};

Bloom::Bloom(int width, int height, int gauss_pass_count, float brightness_threshold)
    : m_bloom_pixels1(width, height),
      m_bloom_pixels2(width, height),
      m_brightness_pass(std::string(vertex_sprite_code_direct),
                        std::string(fragment_brightness_code)),
      m_gauss_vert_pass(std::string(vertex_sprite_code_direct),
                        std::string(fragment_gauss_vert_code)),
      m_gauss_horiz_pass(std::string(vertex_sprite_code_direct),
                         std::string(fragment_gauss_horiz_code)),
      m_combine_pass(std::string(vertex_sprite_code_direct),
                     std::string(fragment_combine_bloom_code)),
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

    setBlendParams({bf::One, bf::One, bf::One, bf::OneMinusSrcAlpha});
    m_screen_sprite.draw(target.getTarget(), m_combine_pass, source, m_bloom_pixels2.getTexture());
}

float TexMip::getAspect() const
{
    return getWidth() / getHeight();
}

float TexMip::getWidth() const
{
    return pixels.getSize().x;
}

float TexMip::getHeight() const
{
    return pixels.getSize().y;
}

TexMip::TexMip(int width, int height, TextureOptions option)
    : pixels(width, height, option)
{
}

BloomPhysical::BloomPhysical(int width, int height, int mip_count, TextureOptions options,
                             float source_weight, float filter_radius, float threshold)
    : m_tex_options(options),
      m_mip_count(mip_count),
      m_filter_radius(filter_radius),
      m_brightness_threshold(threshold),
      m_bright_pixels(width, height, options),
      m_brightness_pass(std::string{vertex_sprite_code_direct},
                        std::string{fragment_brightness_code}),
      m_downsample_cheap_pass(std::string{vertex_sprite_code_direct},
                              std::string{fragment_downsample_cheap_code}),
      m_downsample_pass(std::string{vertex_sprite_code_direct},
                        std::string{fragment_downsample13_code}),
      m_upsample_pass(std::string{vertex_sprite_code_direct},
                      std::string{fragment_upsample_blur_code}),
      m_mixer_pass(std::string{vertex_sprite_code_direct},
                   std::string{fragment_upsample_mix_code})
{
    initMips(width, height, options);
}
BloomPhysical::BloomPhysical(int width, int height, TextureOptions tex_options, BloomParams parameters)
    : m_tex_options(tex_options),
      m_mip_count(parameters.mip_count),
      m_brightness_threshold(parameters.brightness_threshold),
      m_filter_radius(parameters.filter_radius),
      m_source_weight(parameters.source_weight),
      m_bright_pixels(width, height, tex_options),
      m_brightness_pass(std::string{vertex_sprite_code_direct},
                        std::string{fragment_brightness_code}),
      m_downsample_cheap_pass(std::string{vertex_sprite_code_direct},
                              std::string{fragment_downsample_cheap_code}),
      m_downsample_pass(std::string{vertex_sprite_code_direct},
                        std::string{fragment_downsample13_code}),
      m_upsample_pass(std::string{vertex_sprite_code_direct},
                      std::string{fragment_upsample_blur_code}),
      m_mixer_pass(std::string{vertex_sprite_code_direct},
                   std::string{fragment_upsample_mix_code})
{
    initMips(width, height, tex_options);
}

void BloomPhysical::initMips(int width, int height, TextureOptions options)
{

    m_brightness_pass.setUniform("u_threshold", m_brightness_threshold);

    m_mips.clear();
    m_mips.reserve(m_mip_count); //! THIS IS IMPORTANT, OTHERWISE YOU INVALIDATE PREVIOUS MIPS  WHEN PUSHING BACK.
    for (int i = 0; i < m_mip_count; ++i)
    {
        width /= 2;
        height /= 2;
        m_mips.emplace_back(width, height, options);
    }
}
void BloomPhysical::resizeImpl(int w, int h)
{
    m_bright_pixels.resize(w, h);
    m_mips.clear();
    initMips(w, h, m_tex_options);
}

void BloomPhysical::process(Texture &source, Renderer &target)
{
    m_upsample_pass.setUniform("u_filter_radius", m_filter_radius / target.m_view.getScale().x);

    for (auto &mip : m_mips)
    {
        mip.pixels.clear({0, 0, 0, 0});
    }
    m_bright_pixels.clear({0, 0, 0, 0}); 
    m_downsample_cheap_pass.setUniform("u_src_resolution",
                                       glm::vec2(source.getSize().x, source.getSize().y));
    m_screen_sprite.draw(m_mips.at(0).pixels, m_downsample_cheap_pass, source);

    Texture *prev_texture = &m_mips.front().pixels.getTexture();

    int i = 0;
    for (std::size_t mip_id = 1; mip_id < m_mips.size(); ++mip_id)
    {
        TexMip &mip = m_mips.at(mip_id);
        m_downsample_pass.setUniform("u_src_resolution",
                                     glm::vec2(mip.getWidth(), mip.getHeight()));
        m_screen_sprite.draw(mip.pixels, m_downsample_pass, *prev_texture);
        // writeTextureToFile("../", "DS-mip-" + std::to_string(i) + ".png", mip.pixels);
        prev_texture = &mip.pixels.getTexture();
        i++;
    }

    for (int mip_id = (int)m_mips.size() - 2; mip_id >= 0; mip_id--)
    {
        prev_texture = &m_mips.at(mip_id + 1).pixels.getTexture();
        auto &mip = m_mips.at(mip_id);
        mip.pixels.clear({0, 0, 0, 0});
        m_upsample_pass.setUniform("u_aspect_ratio", mip.getAspect());
        m_screen_sprite.draw(mip.pixels, m_upsample_pass, *prev_texture);
    }
    // writeTextureToFile("../", "US-mip-" + std::to_string(1) + ".png", m_mips.at(0).pixels);

    auto &bloom_src = m_mips.front().pixels;
    //    auto &bloom_src = m_bright_pixels;

    m_mixer_pass.setUniform("u_aspect_ratio", m_mips.front().getAspect());
    using bf = BlendFactor;
    setBlendParams({bf::SrcAlpha,  bf::One, bf::One, bf::OneMinusSrcAlpha});
    m_screen_sprite.draw(target.getTarget(), m_mixer_pass, source, bloom_src.getTexture());
}

BloomDualKawase::BloomDualKawase(int width, int height, int mip_count, TextureOptions options,
                                 float source_weight)
    : m_tex_options(options),
      m_bright_pixels(width, height, options),
      m_brightness_pass(std::string{vertex_sprite_code_direct},
                        std::string{fragment_brightness_code}),
      m_downsample_pass(std::string{vertex_sprite_code_direct},
                        std::string{fragment_downsample_code}),
      m_upsample_pass(std::string{vertex_sprite_code_direct},
                      std::string{fragment_upsample_kawase_code}),
      m_mixer_pass(std::string{vertex_sprite_code_direct},
                   std::string{fragment_upsample_mix_kawase_code})
{
    initMips(mip_count, width, height, options);
    m_upsample_pass.setUniform("u_filter_radius", 0.001f);
    m_brightness_pass.setUniform("u_threshold", 0.f);
}

void BloomDualKawase::initMips(int n_levels, int width, int height, TextureOptions options)
{
    m_mips.clear();
    m_mips.reserve(
        n_levels); //! THIS IS IMPORTANT, OTHERWISE YOU INVALIDATE PREVIOUS MIPS  WHEN PUSHING BACK.
    for (int i = 0; i < n_levels; ++i)
    {
        width /= 2;
        height /= 2;
        m_mips.emplace_back(width, height, options);
    }
}

void BloomDualKawase::resizeImpl(int w, int h)
{
    m_bright_pixels.resize(w, h);
    int mip_count = m_mips.size();
    m_mips.clear();
    initMips(mip_count, w, h, m_tex_options);
}
void BloomDualKawase::process(Texture &source, Renderer &target)
{

    m_bright_pixels.clear({0, 0, 0, 0});
    m_screen_sprite.draw(m_bright_pixels, m_brightness_pass, source);
    Texture *prev_texture = &m_bright_pixels.getTexture();

    for (auto &mip : m_mips)
    {
        mip.pixels.clear({0, 0, 0, 0});
    }

    int i = 0;
    for (std::size_t mip_id = 0; mip_id < m_mips.size(); ++mip_id)
    {
        TexMip &mip = m_mips.at(mip_id);
        m_screen_sprite.draw(mip.pixels, m_downsample_pass, *prev_texture);
        // writeTextureToFile("../", "DS-mip-" + std::to_string(i) + ".png", mip.pixels);
        prev_texture = &mip.pixels.getTexture();
        i++;
    }

    for (int mip_id = (int)m_mips.size() - 2; mip_id >= 0; mip_id--)
    {
        prev_texture = &m_mips.at(mip_id + 1).pixels.getTexture();
        auto &mip = m_mips.at(mip_id);
        mip.pixels.clear({0, 0, 0, 0});
        m_screen_sprite.draw(mip.pixels, m_upsample_pass, *prev_texture);
    }
    // writeTextureToFile("../", "US-mip-" + std::to_string(1) + ".png", m_mips.at(0).pixels);

    auto &bloom_src = m_mips.front().pixels;
    m_mixer_pass.setUniform("u_aspect_ratio", m_mips.front().getAspect());
    m_screen_sprite.draw(target.getTarget(), m_mixer_pass, source, bloom_src.getTexture());
}

MapFromHDR::MapFromHDR(int width, int height)
    : m_map_pass(std::string(vertex_sprite_code_direct), std::string(fragment_map_from_hdr))
{
}

void MapFromHDR::process(Texture &source, Renderer &target)
{
    setBlendParams({bf::SrcAlpha, bf::OneMinusSrcAlpha, bf::One, bf::OneMinusSrcAlpha});
    m_screen_sprite.draw(target.getTarget(), m_map_pass, source);
}

LightCombine::LightCombine(int width, int height)
    : m_multiply_texture(width, height),
      m_multiply_pass(std::string(vertex_sprite_code_direct),
                      std::string(fragment_combine_light))
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
      m_horiz_pass(std::string(vertex_sprite_code_direct),
                   std::string(fragment_gauss_horiz_code)),
      m_vert_pass(std::string(vertex_sprite_code_direct),
                  std::string(fragment_gauss_horiz_code)),
      m_edge_detect_pass(std::string(vertex_sprite_code_direct),
                         std::string(fragment_edge_detect_code)),
      m_combine_edges_pass(std::string(vertex_sprite_code_direct),
                           std::string(fragment_edge_combine_code))
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
    m_screen_sprite.draw(target.getTarget(), m_combine_edges_pass, source,
                         m_vert_pixels.getTexture());
    target.drawAll();
    target.m_blend_factors = old_blend_factors;
}
