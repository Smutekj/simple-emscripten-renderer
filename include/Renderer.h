#pragma once

#include "Rect.h"
#include "Batch.h"
#include "Rectangle.h"
#include "Sprite.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "Text.h"

#include <memory>
#include <set>


//! \enum BlendFactor
//! \brief a factor in the color blending equation
//! for details see: \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glBlendFuncSeparate.xhtml
enum class BlendFactor
{
    One = GL_ONE,
    Zero = GL_ZERO,
    SrcAlpha = GL_SRC_ALPHA,
    OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
    SrcColor = GL_SRC_COLOR,
};

//! \struct BlendParams
//! \brief holds info for color blending for OpenGL
//!  the order of parameters corresponds to the order in glBlendFunSeparate(...);
//! for details see: \ref https://registry.khronos.org/OpenGL-Refpages/es3.0/html/glBlendFuncSeparate.xhtml
struct BlendParams
{
    BlendFactor src_factor = BlendFactor::One;
    BlendFactor dst_factor = BlendFactor::OneMinusSrcAlpha;
    BlendFactor src_alpha = BlendFactor::One;
    BlendFactor dst_alpha = BlendFactor::OneMinusSrcAlpha;

    BlendParams() = default;
    BlendParams(BlendFactor src_fact, BlendFactor dst_fact);
    BlendParams(BlendFactor src_fact, BlendFactor dst_fact, BlendFactor src_a, BlendFactor dst_a);
};

class Font;

//! \class Renderer
//! \brief acts as a canvas, with draw functions for:
//! \brief sprites, circles, rectangles, text, and vertex arrays
//! internally batches draw calls. Once the user wants to actually perform the batched draw calls (that is call the GL functions) 
//! drawAll() method should be used to do the actual GL calls and to do the actual screen drawing
class Renderer
{
    using BatchPtr = std::unique_ptr<Batch>;
    using SpriteBatchPtr = std::unique_ptr<SpriteBatch>;

public:
    Renderer(RenderTarget &target);

    void drawSprite(Sprite &sprite, const std::string& shader_id, DrawType draw_type = DrawType::Dynamic);
    void drawSpriteDynamic(Sprite &sprite, const std::string& shader_id);
    void drawText(Text &text, const std::string& shader_id, DrawType draw_type = DrawType::Dynamic);
    void drawLine(Vec2 point_a, Vec2 point_b, float thickness, Color color);
    void drawRectangle(RectangleSimple &r, Color color, const std::string &shader_id = "VertexArrayDefault", DrawType draw_type = DrawType::Dynamic);
    void drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color, DrawType draw_type = DrawType::Dynamic);
    void drawCricleBatched(Vec2 center, float radius, Color color, int n_verts = 32);
    void drawEllipseBatched(Vec2 center, float angle, const utils::Vector2f& scale, Color color, int n_verts = 51, std::string shader_id = "VertexArrayDefault");
    void drawVertices(VertexArray &verts, DrawType draw_type = DrawType::Dynamic, std::shared_ptr<Texture> p_texture = nullptr);
 
    void drawAll();

    utils::Vector2i getTargetSize() const;

    ShaderHolder &getShaders();

    bool hasShader(std::string id);
    void clear(Color c);

    void addShader(std::string id, std::string vertex_path, std::string fragment_path);
    Shader &getShader(const std::string& id);
    Shader *getShaderP(const std::string& id);
    utils::Vector2f getMouseInWorld();
    utils::Vector2i getMouseInScreen();

    View getDefaultView()const;

    bool setShadersPath(std::filesystem::path directory);

private:
    void drawSpriteUnpacked(Vec2 center, Vec2 scale, float angle, ColorByte color, Rect<int> tex_rect, Vec2 texture_size,
                    TextureArray &textures, const std::string& shader_id, DrawType draw_type);
    
    Batch &findBatch(GLuint texture_id, Shader &shader, DrawType draw_type, int num_vertices_inserted);
    Batch &findBatch(TextureArray texture_ids, Shader &shader, DrawType draw_type, int num_vertices_inserted);
    SpriteBatch &findSpriteBatch(GLuint texture_id, Shader &shader, DrawType draw_type);
    SpriteBatch &findSpriteBatch(TextureArray texture_ids, Shader &shader, DrawType draw_type);

    Batch &findFreeBatch(BatchConfig config, Shader &shader, DrawType draw_type, int num_vertices_inserted);
    SpriteBatch &findFreeSpriteBatch(BatchConfig config, Shader &shader, DrawType draw_type);

    BatchPtr createBatch(const BatchConfig &config, Shader &shader, DrawType draw_type);

    bool checkShader(const std::string &shader_id);

public:
    View m_view;                //!< view defines what part of the world we "look at" and thus draw
    Rect<float> m_viewport;        //!< viewport defines part of the window, we draw into
    BlendParams m_blend_factors; //!< OpenGL factors of the blend equation

private:
    ShaderHolder m_shaders; //!< stores shaders that we can use in this canvas (will probably just use singleton later on...)

    std::unordered_map<BatchConfig, std::vector<BatchPtr>> m_config2batches; //!< stores batches
    std::unordered_map<BatchConfig, std::vector<SpriteBatchPtr>> m_config2sprite_batches;

    RenderTarget &m_target;     //!< the actual draw target
};