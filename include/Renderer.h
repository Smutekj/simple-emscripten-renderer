#pragma once

#include "Rect.h"
#include "Batch.h"
#include "Rectangle.h"
#include "Texture.h"
#include "RenderTarget.h"

#include <memory>
#include <set>

enum class BlendFactor
{
    One = GL_ONE,
    Zero = GL_ZERO,
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    SrcColor = GL_SRC_COLOR,
};

//! \struct holds info for color blending for OpenGL
//! \brief the order of parameters corresponds to the order in
//! \brief glBlendFunSeparate(...);
//! \brief https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glBlendFuncSeparate.xhtml
struct BlendParams
{
    BlendFactor src_factor = BlendFactor::One;
    BlendFactor dst_factor = BlendFactor::OneMinusSrcAlpha;
    BlendFactor src_alpha = BlendFactor::One;
    BlendFactor dst_alpha = BlendFactor::OneMinusSrcAlpha;

    BlendParams() = default;
    BlendParams(BlendFactor src_fact, BlendFactor dst_fact)
        : src_factor(src_fact), dst_factor(dst_fact)
    {
    }
    BlendParams(BlendFactor src_fact, BlendFactor dst_fact, BlendFactor src_a, BlendFactor dst_a)
        : src_factor(src_fact), dst_factor(dst_fact), src_alpha(src_a), dst_alpha(dst_a)
    {
    }
};

struct Rectangle2 : public Transform
{
    float width = 1;
    float height = 1;
};

struct Sprite2 : public Rectangle2
{

    Sprite2(Texture &texture)
        : m_texture(&texture),
          m_tex_rect(0, 0, (int)texture.getSize().x, (int)texture.getSize().y)
    {
        m_texture_handles.at(0) = texture.getHandle();
        m_tex_size = texture.getSize();
    }

    void setTexture(Texture &texture)
    {
        setTexture(0, texture);
    }

    void setTexture(int slot, Texture &texture)
    {
        m_texture = &texture;
        m_texture_handles.at(slot) = texture.getHandle();
        m_tex_rect = {0, 0, (int)texture.getSize().x, (int)texture.getSize().y};
        m_tex_size = texture.getSize();
    }
    Texture *m_texture = nullptr;
    std::array<GLuint, N_MAX_TEXTURES> m_texture_handles = {0, 0};
    utils::Vector2i m_tex_size = {0, 0};
    Rect<int> m_tex_rect;
    ColorByte m_color = {255, 255, 255, 255};
};

class Font;

class Text : public Transform
{

public:
    Text(std::string text = "");
    void setFont(std::shared_ptr<Font> new_font);
    std::shared_ptr<Font> getFont();
    void setText(const std::string &new_text);
    const std::string &getText() const;
    void setColor(ColorByte new_color);
    const ColorByte &getColor() const;

private:
    std::weak_ptr<Font> m_font;
    std::string m_text = "";
    ColorByte m_color = {255, 255, 255, 255};
};

class Renderer
{
    using BatchPtr = std::unique_ptr<Batch>;
    using SpriteBatchPtr = std::unique_ptr<SpriteBatch>;

public:
    Renderer(RenderTarget &target);

    void drawSprite(Vec2 center, Vec2 scale, float angle, ColorByte color, Rect<int> tex_rect,
                    Texture &texture, std::string shader_id, GLenum draw_type);
    void drawSprite(Sprite2 &sprite, std::string shader_id, GLenum draw_type);
    void drawSprite(Vec2 center, Vec2 scale, float angle, ColorByte color, Rect<int> tex_rect, Vec2 texture_size,
                    std::array<GLuint, N_MAX_TEXTURES> &textures, std::string shader_id, GLenum draw_type);
    void drawText(std::string text, Vec2 center, Vec2 scale, float angle, Font &font,
                  std::string shader_id, GLenum draw_type);
    void drawText(Text &text, std::string shader_id, GLenum draw_type);

    void drawLine(Vec2 point_a, Vec2 point_b, float thickness, Color color);

    void drawRectangle(Rectangle2 &r, Color color, const std::string &shader_id = "VertexArrayDefault", GLenum draw_type = GL_DYNAMIC_DRAW);
    void drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color, GLenum draw_type = GL_DYNAMIC_DRAW);
    void drawCricleBatched(Vec2 center, float radius, Color color, int n_verts = 51);
    void drawCricleBatched(Vec2 center, float angle, float radius_a, float radius_b, Color color, int n_verts = 51);
    void drawVertices(VertexArray &verts, GLenum draw_type = GL_DYNAMIC_DRAW, std::shared_ptr<Texture> p_texture = nullptr);

    void drawAll();

    utils::Vector2i getTargetSize() const;

    ShaderHolder &getShaders();

    bool hasShader(std::string id);
    void clear(Color c);

    void addShader(std::string id, std::string vertex_path, std::string fragment_path);
    Shader &getShader(std::string id);
    utils::Vector2f getMouseInWorld();

public:
    View m_view;
    Rect<float> m_viewport;
    BlendParams m_blend_factors;

private:
    void drawSpriteStatic(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect,
                          Texture &texture, Shader &shader);
    Batch &findBatch(GLuint texture_id, Shader &shader, GLenum draw_type, int num_vertices_inserted);
    Batch &findBatch(std::array<GLuint, N_MAX_TEXTURES> texture_ids, Shader &shader, GLenum draw_type, int num_vertices_inserted);
    SpriteBatch &findSpriteBatch(GLuint texture_id, Shader &shader, GLenum draw_type);
    SpriteBatch &findSpriteBatch(std::array<GLuint, N_MAX_TEXTURES> texture_ids, Shader &shader, GLenum draw_type);

    Batch &findFreeBatch(BatchConfig config, Shader &shader, GLenum draw_type, int num_vertices_inserted);
    SpriteBatch &findFreeSpriteBatch(BatchConfig config, Shader &shader, GLenum draw_type);

    BatchPtr createBatch(const BatchConfig &config, Shader &shader, GLenum draw_type);

    ShaderHolder m_shaders;

    std::unordered_map<BatchConfig, int> m_config2next_free_batch;
    std::unordered_map<BatchConfig, std::vector<BatchPtr>> m_config2batches;
    std::unordered_map<BatchConfig, std::vector<SpriteBatchPtr>> m_config2sprite_batches;

    RenderTarget &m_target;
};