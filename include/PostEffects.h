#pragma once

#include <FrameBuffer.h>
#include <Renderer.h>
#include <Rectangle.h>
#include <Shader.h>

class PostEffect
{
public:
    virtual ~PostEffect() = default;

    virtual void process(Texture &source, Renderer &target) = 0;
    virtual void resizeImpl(int w, int h) {};

    void resize(int w, int h);
};

class Bloom : public PostEffect
{

public:
    Bloom(int width, int height, int gauss_pass_count = 3, float brightness_threshold = 1.f);

    void process(Texture &source, Renderer &target) override;

private:
    FrameBuffer m_bloom_pixels1;
    FrameBuffer m_bloom_pixels2;

    Shader m_brightness_pass;
    Shader m_gauss_vert_pass;
    Shader m_gauss_horiz_pass;
    Shader m_combine_pass;
    ScreenSprite m_screen_sprite;

    int m_gauss_pass_count;
    float m_brightness_threshold;
};

struct TexMip
{
    TexMip(int width, int height, TextureOptions option);

    float getAspect() const;
    float getWidth() const;
    float getHeight() const;

    FrameBuffer pixels;
};

class BloomDualKawase : public PostEffect
{

public:
    BloomDualKawase(int width, int height, int mip_count = 3, TextureOptions options = {}, float source_weight = 1.f);

    void process(Texture &source, Renderer &target) override;
    void resizeImpl(int w, int h) override;
    void initMips(int n_levels, int width, int height, TextureOptions option);

private:
    TextureOptions m_tex_options;
    FrameBuffer m_bright_pixels;
    ScreenSprite m_screen_sprite;

    std::vector<TexMip> m_mips;

    Shader m_brightness_pass;
    Shader m_downsample_pass;
    Shader m_upsample_pass;
    Shader m_mixer_pass;
};

struct BloomParams
{
    float source_weight = 1.;
    float filter_radius = 0.0005;
    float brightness_threshold = 1.f;
    int mip_count = 3;
};

class BloomPhysical : public PostEffect
{

public:
    BloomPhysical(int width, int height, int mip_count = 3, TextureOptions options = {}, float source_weight = 1.f, float filter_radius = 0.5f, float brightness_threshold = 1.f);
    BloomPhysical(int width, int height, TextureOptions options = {}, BloomParams params = {});

    void process(Texture &source, Renderer &target) override;
    void resizeImpl(int w, int h) override;
    
    void initMips(int width, int height, TextureOptions option);

private:
    TextureOptions m_tex_options;
    int m_mip_count;
    float m_brightness_threshold;
    float m_filter_radius;
    float m_source_weight;

    FrameBuffer m_bright_pixels;
    ScreenSprite m_screen_sprite;

    std::vector<TexMip> m_mips;

    Shader m_brightness_pass;
    Shader m_downsample_pass;
    Shader m_downsample_cheap_pass;
    Shader m_upsample_pass;
    Shader m_mixer_pass;
};

class EdgeDetect : public PostEffect
{
public:
    EdgeDetect(int width, int height);

    void process(Texture &source, Renderer &target) override;

private:
    FrameBuffer m_vert_pixels;
    FrameBuffer m_horiz_pixels;

    ScreenSprite m_screen_sprite;

    Shader m_vert_pass;
    Shader m_edge_detect_pass;
    Shader m_horiz_pass;
    Shader m_combine_edges_pass;
};
class LightCombine : public PostEffect
{

public:
    LightCombine(int width, int height);

    void process(Texture &source, Renderer &target) override;

private:
    Shader m_multiply_pass;
    FrameBuffer m_multiply_texture;
    ScreenSprite m_screen_sprite;
};

class MapFromHDR : public PostEffect
{
public:
    MapFromHDR(int width, int height);

    void process(Texture &source, Renderer &target) override;

private:
    Shader m_map_pass;
    ScreenSprite m_screen_sprite;
};
