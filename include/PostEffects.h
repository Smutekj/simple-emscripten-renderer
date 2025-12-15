#pragma once

#include <FrameBuffer.h>
#include <Renderer.h>
#include <Rectangle.h>

class PostEffect
{
public:
    virtual void process(Texture &source, Renderer &target) = 0;
    void resize(int w, int h)
    {
        if (w == 0 || h == 0)
        {
            return;
        }
        resizeImpl(w, h);
    };
    virtual void resizeImpl(int w, int h) {};

    virtual ~PostEffect() = default;

protected:
};

class Bloom : public PostEffect
{

public:
    Bloom(int width, int height, int gauss_pass_count = 3, float brightness_threshold = 1.f);
    virtual void process(Texture &source, Renderer &target) override;
    virtual ~Bloom() {}

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


class BloomPhysical : public PostEffect
{

public:
    BloomPhysical(int width, int height, int mip_count = 3, int gauss_pass_count = 3, TextureOptions options = {}, std::string final_shader = "combineLightBloom");

    virtual void process(Texture &source, Renderer &target) override;
    virtual ~BloomPhysical() {};
    virtual void resizeImpl(int w, int h) override
    {
        m_bright_pixels.resize(w, h);
        int mip_count = m_mips.size();
        m_mips.clear();
        initMips(mip_count, w, h, m_options);
    }

    void initMips(int n_levels, int width, int height, TextureOptions option);
    struct TexMip
    {
        TexMip(int width, int height, TextureOptions option);

        float getAspect() const;
        float getWidth() const ;
        float getHeight() const;

        FrameBuffer pixels;
    };

private:
    TextureOptions m_options;
    FrameBuffer m_bright_pixels;
    ScreenSprite m_screen_sprite;

    std::vector<TexMip> m_mips;

    Shader m_brightness_pass;
    Shader m_downsample_pass;
    Shader m_upsample_pass;
    Shader m_mixer_pass;
};


class EdgeDetect : public PostEffect
{

public:
    EdgeDetect(int width, int height);

    virtual void process(Texture &source, Renderer &target) override;
    virtual ~EdgeDetect() = default;

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

    virtual void process(Texture &source, Renderer &target) override;
    virtual ~LightCombine() = default;

private:
    Shader m_multiply_pass;
    FrameBuffer m_multiply_texture;
    ScreenSprite m_screen_sprite;
};


class BloomFinal : public PostEffect
{

public:
    BloomFinal(int width, int height, int mip_count = 3, int gauss_pass_count = 2, float brightness_threshold = 0.f, TextureOptions options = {});

    virtual void process(Texture &source, Renderer &target) override;
    virtual ~BloomFinal() {};

private:
    void initMips(int n_levels, int width, int height, TextureOptions option);
    struct TexMip
    {
        TexMip(int width, int height, TextureOptions option)
            : pixels(width, height, option),  pixels_tmp(width, height, option)
        {
        }
        FrameBuffer pixels;
        FrameBuffer pixels_tmp;
    };

private:
    std::vector<TexMip> m_mips;
    
    Shader m_brightness_pass;
    Shader m_gauss_vert_pass;
    Shader m_gauss_horiz_pass;
    Shader m_downsample_pass;
    Shader m_combine_pass;
    Shader m_full_pass;
    ScreenSprite m_screen_sprite;
    
    int m_gauss_pass_count;
    float m_brightness_threshold;
};