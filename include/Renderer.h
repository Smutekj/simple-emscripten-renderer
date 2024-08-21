#pragma once

#include "Rect.h"
#include "Batch.h"
#include "Rectangle.h"
#include "Texture.h"
#include "RenderTarget.h"

#include <memory>
#include <set>

struct Rectangle2 : public Transform
{

    float width = 1;
    float height = 1;
};

struct Sprite2 : public Rectangle2
{

    Sprite2(Texture &texture) 
    :
     m_texture(&texture),
     m_tex_rect(0, 0, (int)texture.getSize().x, (int)texture.getSize().y)
      { m_texture_handles.at(0) = texture.getHandle(); }

    void setTexture(Texture &texture)
    {
        setTexture(0, texture);
    }

    void setTexture(int slot, Texture &texture)
    {
        m_texture = &texture;
        m_texture_handles.at(slot) = texture.getHandle();
    }
    Texture* m_texture = nullptr;
    std::array<GLuint, N_MAX_TEXTURES> m_texture_handles = {0,0};
    Rect<int> m_tex_rect;
};

class Renderer
{
    using BatchPtr = std::unique_ptr<Batch>;
    using SpriteBatchPtr = std::unique_ptr<SpriteBatch>;
public:
    Renderer(RenderTarget &target);

    void drawSprite(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect,
                    Texture &texture, std::string shader_id, GLenum draw_type);
    void drawSprite(Sprite2 &sprite, std::string shader_id, GLenum draw_type);
    void drawSprite(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect, Vec2 texture_size,
                    std::array<GLuint, N_MAX_TEXTURES> &textures, std::string shader_id, GLenum draw_type);

    void drawLine(Vec2 point_a, Vec2 point_b, float thickness, Color color);

    void drawRectangle(Rectangle2 &r, Color color, GLenum draw_type);
    void drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color, GLenum draw_type = GL_DYNAMIC_DRAW);
    void drawCricleBatched(Vec2 center, float radius, Color color, int n_verts = 51);

    void drawAll();

    utils::Vector2i getTargetSize()const;

    ShaderHolder &getShaders();

    bool hasShader(std::string id);
    void clear(Color c);

    void addShader(std::string id, std::string vertex_path, std::string fragment_path);
    Shader &getShader(std::string id);
    utils::Vector2f getMouseInWorld();

public:
    View m_view;
    Rect<float> m_viewport;

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