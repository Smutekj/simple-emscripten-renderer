#pragma once

#include "Rect.h"
#include "RenderTarget.h"
#include "BatchConfig.h"
#include "ShaderHolder.h"
#include "View.h"

#include <memory>
#include <set>

#include "Batch.h"
#include "BlendParams.h"
#include "Sprite.h"

class Text;
class Texture;
class Font;
class RectangleSimple;

//! \class Renderer
//! \brief acts as a canvas, with draw functions for:
//! \brief sprites, circles, rectangles, text, and vertex arrays
//! internally batches draw calls. Once the user wants to actually perform the batched draw calls (that is call the GL functions)
//! drawAll() method should be used to do the actual GL calls and to do the actual screen drawing
class Renderer
{

public:
    explicit Renderer(RenderTarget &target);

    void drawSprite(Sprite &sprite, const std::string &shader_id = "SpriteDefault");
    void drawText(const Text &text, const std::string &shader_id = "TextDefault");
    void drawText2(const Text &text, const std::string &shader_id = "TextDefault2");
    void drawLine(Vec2 point_a, Vec2 point_b, float thickness, Color color);
    void drawRectangle(RectangleSimple &r, const std::string &shader_id = "VertexArrayDefault");
    void drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color);
    void drawCricleBatched(Vec2 center, float radius, Color color, int n_verts = 32);
    void drawPartialCircle(Vec2 center, float radius, float angle_start, float angle_end, Color color, int n_verts = 32);
    void drawEllipseBatched(Vec2 center, float angle, const utils::Vector2f &scale, Color color, int n_verts = 51, std::string shader_id = "VertexArrayDefault");
    void drawVertices(std::vector<Vertex> &verts, const std::string &shader_id = "VertexArrayDefault", std::shared_ptr<Texture> p_texture = nullptr);

    template <class DrawableT>
    void drawBatched(DrawableT &drawable, const std::string &shader_id, TextureArray textures = {0, 0});

    template <class DrawableT>
    void registerDrawable();

    void drawAll();
    void drawAllInto(RenderTarget &target);
    void resetBatches();

    utils::Vector2i getTargetSize() const;
    RenderTarget &getTarget() const;

    void clear(Color c);

    ShaderHolder &getShaders();
    bool hasShader(std::string id);
    void addShader(std::string id, std::string vertex_path, std::string fragment_path);
    Shader &getShader(const std::string &id);
    Shader *getShaderP(const std::string &id);
    bool setShadersPath(std::filesystem::path directory);

    utils::Vector2f getMouseInWorld();
    utils::Vector2i getMouseInScreen();

    View getDefaultView() const;

private:
    void drawSpriteUnpacked(Vec2 center, Vec2 scale, float angle, ColorByte color, Rect<int> tex_rect, Vec2 texture_size,
                            TextureArray &textures, const std::string &shader_id);

    bool checkShader(const std::string &shader_id);

public:
    View m_view;                 //!< view defines what part of the world we "look at" and thus draw
    Rect<float> m_viewport;      //!< viewport defines part of the window, we draw into
    BlendParams m_blend_factors; //!< OpenGL factors of the blend equation

private:
    //! dirty flags
    bool m_blend_factors_changed = true;
    bool m_viewport_changed = true;

    ShaderHolder m_shaders; //!< stores shaders that we can use in this canvas (will probably just use singleton later on...)

    BatchRegistry m_batches;

    RenderTarget &m_target; //!< the actual draw target
};

//! \brief utility function to pass a texture into a buffer via a shader
void renderToTraget(Renderer &target, const Texture &source, const std::string &shader_id);
void renderToTarget(RenderTarget &target, const Texture &source, Shader &shader);

template <class DrawableT>
void Renderer::drawBatched(DrawableT &drawable, const std::string &shader_id, TextureArray textures)
{
    Shader &shader = m_shaders.get(shader_id);

    BatchConfig config(textures, &shader);
    m_batches.pushInstance(drawable.getInstance(), config);
}

template <class DrawableT>
void Renderer::registerDrawable()
{
    m_batches.registerBatch<typename DrawableT::VertexType,
                            typename DrawableT::InstanceType>(
        DrawableT::makeBatch());
}

template <class Derived>
struct Drawable
{
    // Get the instance data from derived class
    // typename Derived::InstanceType &getInstanceData()
    // {
    //     return static_cast<Derived *>(this)->getInstanceData();
    // }

    // const typename Derived::InstanceType &getInstanceData() const
    // {
    //     return static_cast<const Derived *>(this)->getInstanceData();
    // }

    // Static polymorphism for batch creation
    static BatchRegistry::BatchMaker makeBatch()
    {
        return Derived::makeBatchImpl();
    }
};

struct SpriteVertexLayout
{

    static constexpr std::size_t size = 2 * sizeof(utils::Vector2f);

    static constexpr float QUAD_VERTICES[6 * 4] = {
        -1, -1, 0, 0,
        -1, +1, 0, 1,
        +1, -1, 1, 0,
        +1, +1, 1, 1,
        +1, -1, 1, 0,
        -1, +1, 0, 1};

    static VAOId::AttributeList getVertexAttributes()
    {
        return {
            makeAttribute<utils::Vector2f>(), // pos
            makeAttribute<utils::Vector2f>()  // tex_coord
        };
    }

    static std::vector<std::byte> getVertexData()
    {
        std::vector<std::byte> data;
        const auto *begin = reinterpret_cast<const std::byte *>(QUAD_VERTICES);
        const auto *end = begin + sizeof(QUAD_VERTICES);
        data.insert(data.end(), begin, end);
        return data;
    }
};


struct RoundedRectVertexLayout
{

    static constexpr std::size_t size = 2 * sizeof(utils::Vector2f);

    static constexpr float QUAD_VERTICES[6 * 4] = {
        -1, -1, 0, 0,
        -1, +1, 0, 1,
        +1, -1, 1, 0,
        +1, +1, 1, 1,
        +1, -1, 1, 0,
        -1, +1, 0, 1};

    static VAOId::AttributeList getVertexAttributes()
    {
        return {
            makeAttribute<utils::Vector2f>(), // pos
            makeAttribute<utils::Vector2f>()  // tex_coord
        };
    }

    static std::vector<std::byte> getVertexData()
    {
        std::vector<std::byte> data;
        const auto *begin = reinterpret_cast<const std::byte *>(QUAD_VERTICES);
        const auto *end = begin + sizeof(QUAD_VERTICES);
        data.insert(data.end(), begin, end);
        return data;
    }
};

struct BlurredRect : public Drawable<BlurredRect>
{
    using VertexType = SpriteVertexLayout;
    using InstanceType = BlurredRect;

    static BatchRegistry::BatchMaker makeBatchImpl()
    {
        VAOId layout;

        layout.instanced_attributes = {
            makeAttribute<utils::Vector2f>(),
            makeAttribute<utils::Vector2f>(),
            makeAttribute<float>(),
            makeAttribute<ColorByte>(),
            makeAttribute<ColorByte>(),
            makeAttribute<float>(),
            makeAttribute<float>(),
            makeAttribute<float>(),
        };

        layout.vertex_attirbutes = SpriteVertexLayout::getVertexAttributes();
        layout.vertices_size = SpriteVertexLayout::size;

        layout.instance_size = sizeof(BlurredRect);
        layout.max_vertex_buffer_count = 6; //! vertices are just a square
        layout.max_instance_count = 40000;

        auto vertex_data = SpriteVertexLayout::getVertexData();
        return [=]()
        { return std::make_unique<InstancedBatch>(vertex_data, layout); };
    }

    const BlurredRect &getInstance() const
    {
        return *this;
    }

    utils::Vector2f pos;
    utils::Vector2f scale = {1.f, 1.f};
    float angle = 0.f;
    ColorByte fill_color;
    ColorByte outline_color;
    float corner_radius = 0.f;
    float outline_width = 0.f;
    float blur = 0.f;
};
