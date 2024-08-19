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
    // Renderer();
    // Renderer(int width, int height, bool offscreen = false);
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

    auto getTargetSize()const
    {
        return m_target.getSize();
    }

    ShaderHolder &getShaders()
    {
        return m_shaders;
    }

    bool hasShader(std::string id)
    {
        return m_shaders.getShaders().count(id) > 0;
    }

    void clear(Color c)
    {
        m_target.clear(c);
    }

    void addShader(std::string id, std::string vertex_path, std::string fragment_path)
    {
        m_shaders.load(id, vertex_path, fragment_path);
    }

    Shader &getShader(std::string id)
    {
        return m_shaders.get(id);
    }

    utils::Vector2f getMouseInWorld()
    {
        int mouse_coords[2];

        auto m = glm::inverse(m_view.getMatrix());
        auto button = SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);
        glm::vec4 world_coords = m * glm::vec4(
                                         2. * mouse_coords[0] / m_target.getSize().x - 1.,
                                         -2. * mouse_coords[1] / m_target.getSize().y + 1.f, 0, 1);
        return {world_coords.x, world_coords.y};
    }

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

    Batch &findFreeBatch(BatchConfig config, Shader &shader, GLenum draw_type, int num_vertices_inserted)
    {
        auto &batches = m_config2batches.at(config);
        for (auto &batch : batches)
        {
            if (batch->getFreeVerts() >= num_vertices_inserted)
            {
                return *batch;
            }
        }
        //! there is no free batch so we create a new one;
        batches.push_back(createBatch(config, shader, draw_type));
        return *batches.back();
    }
    SpriteBatch &findFreeSpriteBatch(BatchConfig config, Shader &shader, GLenum draw_type)
    {
        auto &batches = m_config2sprite_batches.at(config);
        auto it = std::find_if(batches.begin(), batches.end(), [](auto &batch)
                               { return batch->getFreeVerts() >= 1; });
        if (it != batches.end())
        {
            return **it;
        }
        m_config2sprite_batches.at(config).push_back(std::make_unique<SpriteBatch>(config, shader));
        return *batches.back();
    }

    BatchPtr createBatch(const BatchConfig &config, Shader &shader, GLenum draw_type);

    ShaderHolder m_shaders;
    // TextureHolder m_textures;

    std::unordered_map<BatchConfig, int> m_config2next_free_batch;
    std::unordered_map<BatchConfig, std::vector<BatchPtr>> m_config2batches;
    std::unordered_map<BatchConfig, std::vector<SpriteBatchPtr>> m_config2sprite_batches;

    RenderTarget &m_target;

    Texture t_test;
};