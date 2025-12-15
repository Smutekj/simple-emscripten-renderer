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
    Bloom(int width, int height);
    virtual void process(Texture &source, Renderer &target) override;
    virtual ~Bloom() {}

private:
    FrameBuffer m_bloom_pass1;
    FrameBuffer m_bloom_pass2;

    Renderer m_bloom_renderer1;
    Renderer m_bloom_renderer2;
};

class BloomFinal : public PostEffect
{

public:
    BloomFinal(int width, int height, int mip_count = 3, int gauss_pass_count = 3, TextureOptions options = {}, std::string final_shader = "combineLightBloom");

    virtual void process(Texture &source, Renderer &target) override;
    virtual ~BloomFinal() {};

    void initMips(int n_levels, int width, int height, TextureOptions option);
    struct TexMip
    {
        TexMip(int width, int height, TextureOptions option)
            : pixels(width, height, option), canvas(pixels), pixels_tmp(width, height, option), canvas_tmp(pixels_tmp)
        {
        }
        FrameBuffer pixels;
        Renderer canvas;
        FrameBuffer pixels_tmp;
        Renderer canvas_tmp;
    };

private:
    std::string m_final_shader;
    int m_gauss_pass_count;
    std::vector<TexMip> m_mips;
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


class BloomSmoke : public PostEffect
{

public:
    BloomSmoke(int width, int height);

    virtual void process(Texture &source, Renderer &target) override;
    virtual ~BloomSmoke() = default;

private:
    FrameBuffer m_bloom_pass1;
    FrameBuffer m_bloom_pass2;
    FrameBuffer m_downsampled_pixels3;
    FrameBuffer m_downsampled_pixels33;

    Renderer m_bloom_renderer1;
    Renderer m_bloom_renderer2;
    Renderer m_downsampler3;
    Renderer m_downsampler33;

    std::vector<FrameBuffer> m_mips;
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

