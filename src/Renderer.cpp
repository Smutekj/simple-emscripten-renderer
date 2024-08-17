#include "Renderer.h"

#include "Rectangle.h"
#include "Texture.h"

#include <chrono>
#include <numbers>

Renderer::Renderer(RenderTarget &target)
    : m_target(target), m_viewport(0.f, 0.f, 1.f, 1.f)
{
}

void Renderer::drawSprite(Sprite2 &sprite,  std::string shader_id, GLenum draw_type)
{
    // drawSprite(sprite.getPosition(), sprite.getScale(), sprite.getRotation(),
    //            sprite.m_tex_rect, sprite.m_texture, shader_id, draw_type);
    drawSprite(sprite.getPosition(), sprite.getScale(), sprite.getRotation(),
               sprite.m_tex_rect, sprite.m_texture->getSize(), sprite.m_texture_handles, shader_id, draw_type);
}

void Renderer::drawSprite(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect,
                          Texture &texture, std::string shader_id, GLenum draw_type)
{
    auto &shader = m_shaders.get(shader_id);

    if (draw_type == GL_STATIC_DRAW)
    {
        drawSpriteStatic(center, scale, angle, tex_rect, texture, shader);
        return;
    }

    Trans t;
    t.angle = angle;
    t.trans = center;
    t.scale = scale;

    //! normalize the texture rectangle to be between [0,1] just as OpenGL likes it
    auto tex_size = texture.getSize();
    Rect<float> tex_rect_norm = {tex_rect.pos_x / tex_size.x, tex_rect.pos_y / tex_size.y,
                                 tex_rect.width / tex_size.x, tex_rect.height / tex_size.y};
    t.tex_coords = {tex_rect_norm.pos_x, tex_rect_norm.pos_y};
    t.tex_size = {tex_rect_norm.width, tex_rect_norm.height};

    auto &batch = findSpriteBatch(texture.getHandle(), shader, GL_DYNAMIC_DRAW);
    if (batch.addSprite(t))
    {
    }
}

void Renderer::drawSprite(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect,
                          Vec2 texture_size, std::array<GLuint, N_MAX_TEXTURES>& texture_handles,
                            std::string shader_id, GLenum draw_type)
{
    auto &shader = m_shaders.get(shader_id);

    if (draw_type == GL_STATIC_DRAW)
    {
        std::cout <<"STATIC DRAW NOT SUPPORTED YET!!!";
        return;
    }

    Trans t;
    t.angle = angle;
    t.trans = center;
    t.scale = scale;

    //! normalize the texture rectangle to be between [0,1] just as OpenGL likes it
    auto tex_size = texture_size;
    Rect<float> tex_rect_norm = {tex_rect.pos_x / tex_size.x, tex_rect.pos_y / tex_size.y,
                                 tex_rect.width / tex_size.x, tex_rect.height / tex_size.y};
    t.tex_coords = {tex_rect_norm.pos_x, tex_rect_norm.pos_y};
    t.tex_size = {tex_rect_norm.width, tex_rect_norm.height};

    auto &batch = findSpriteBatch(texture_handles, shader, GL_DYNAMIC_DRAW);
    if (batch.addSprite(t))
    {
    }
}

void Renderer::drawSpriteStatic(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect,
                                Texture &texture, Shader &shader)
{
    auto &batch = findBatch(texture.getHandle(), shader, GL_STATIC_DRAW, 4);

    Sprite s(texture, shader);
    s.setRotation(angle);
    s.setScale(scale.x / 2.f, scale.y / 2.f);
    s.setPosition(center.x, center.y);
    s.setTextureRect(tex_rect);

    auto verts = s.getVerts();
    int next_ind = batch.getLastInd();
    batch.pushVertexArray(verts);
    batch.pushVertex(next_ind);
    batch.pushVertex(next_ind + 2);
}

void Renderer::drawLine(Vec2 point_a, Vec2 point_b, float thickness, Color color)
{
    Rectangle r(m_shaders.get("ShapeDefault"));
    Vec2 dr = {point_b.x - point_a.x, point_b.y - point_a.y};
    r.setRotation(std::atan2(dr.y, dr.x));
    r.setScale(thickness, std::sqrt(dr.x * dr.x + dr.y * dr.y) / 2.f);
    r.setPosition((point_a.x + point_b.x) / 2.f, (point_a.y + point_b.y) / 2.f);
    r.setColor(color);

    m_target.bind();
    r.draw(0, m_view);
}

void Renderer::drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color,
                               GLenum draw_type)
{
    auto &batch = findBatch(0, m_shaders.get("VertexArrayDefault"), draw_type, 6);

    std::vector<Vertex> verts(6); //! this should be static?
    verts[0] = {{-1.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f}};
    verts[1] = {{1.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 0.f}};
    verts[2] = {{1.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f}};
    verts[3] = {{-1.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 1.f}};
    verts[4] = {{-1.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f}};
    verts[5] = {{1.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f}};

    Vec2 dr = {point_b.x - point_a.x, point_b.y - point_a.y};
    Transform matrix;
    matrix.setRotation(glm::degrees(std::atan2(dr.y, dr.x)));
    matrix.setScale(std::sqrt(dr.x * dr.x + dr.y * dr.y) / 2.f, thickness);
    matrix.setPosition((point_a.x + point_b.x) / 2.f, (point_a.y + point_b.y) / 2.f);

    //! set color and transform positions
    for (auto &v : verts)
    {
        matrix.transform(v.pos);
        v.color = color;
    }

    batch.pushVertexArray(verts);
    // batch.pushVertex(0);
    // batch.pushVertex(2);
}

void Renderer::drawRectangle(Rectangle2 &r, Color color, GLenum draw_type)
{
    auto &batch = findBatch(0, m_shaders.get("VertexArrayDefault"), draw_type, 6);

    std::vector<Vertex> verts(6); //! this should be static?
    verts[0] = {{-r.width / 2.f, -r.height / 2.f}, color, {0.f, 0.f}};
    verts[1] = {{+r.width / 2.f, -r.height / 2.f}, color, {1.f, 0.f}};
    verts[2] = {{+r.width / 2.f, +r.height / 2.f}, color, {1.f, 1.f}};
    verts[3] = {{-r.width / 2.f, +r.height / 2.f}, color, {0.f, 1.f}};
    verts[4] = {{-r.width / 2.f, -r.height / 2.f}, color, {0.f, 0.f}};
    verts[5] = {{+r.width / 2.f, +r.height / 2.f}, color, {1.f, 1.f}};

    //! set color and transform positions
    for (auto &v : verts)
    {
        r.transform(v.pos);
    }

    batch.pushVertexArray(verts);
}
void Renderer::drawCricleBatched(Vec2 center, float radius, Color color, int n_verts)
{
    auto &batch = findBatch(0, m_shaders.get("VertexArrayDefault"), GL_DYNAMIC_DRAW, n_verts);

    auto pi = std::numbers::pi_v<float>;

    auto n_verts_circumference = n_verts - 1;
    batch.pushVertex({center, color, {0, 0}});
    for (int i = 0; i < n_verts_circumference; ++i)
    {
        float x = std::cos(2.f * i * pi / 50.f);
        float y = std::sin(2.f * i * pi / 50.f);
        Vertex v = {{center.x + x * radius, center.y + y * radius}, color, {x, y}};
        batch.pushVertex(v);
    }

    auto &indices = batch.m_indices;
    indices.resize(indices.size() - n_verts); //! get rid of indices just created by pushVertex
    auto last_index = batch.getLastInd() - n_verts;
    //! make triangles by specifying indices
    for (IndexType i = 0; i < n_verts_circumference; ++i)
    {
        indices.push_back(last_index);
        indices.push_back((i) % n_verts_circumference + last_index + 1);
        indices.push_back((i + 1) % n_verts_circumference + last_index + 1);
    }
}

void Renderer::drawAll()
{
    m_target.bind(); //! binds the corresponding opengl framebuffer (or window)
    //! set proper view

    glViewport(m_viewport.pos_x * m_target.getSize().x,
               m_viewport.pos_y * m_target.getSize().y,
               m_viewport.width * m_target.getSize().x,
               m_viewport.height * m_target.getSize().y);

    std::vector<BatchConfig> to_delete;
    for (auto &[config, batches] : m_config2sprite_batches)
    {
        for (auto &batch : batches)
        {
            batch->flush2(m_view);
        }
        //! clear dynamic batches
        if (config.draw_type != GL_STATIC_DRAW)
        {
            to_delete.push_back(config);
        }
    }

    while (!to_delete.empty())
    {
        //     m_config2sprite_batches.erase(to_delete.back());
        to_delete.pop_back();
    }
    for (auto &[config, batches] : m_config2batches)
    {

        for (auto &batch : batches)
        {
            batch->flush(m_view);
        }

        //! clear dynamic batches
        if (config.draw_type != GL_STATIC_DRAW)
        {
            to_delete.push_back(config);
        }
    }

    while (!to_delete.empty())
    {
        assert(m_config2batches.count(to_delete.back()) != 0);
        // m_config2batches.at(to_delete.back()).clear();
        // m_config2batches.erase(to_delete.back());
        // m_config2next_free_batch.at(to_delete.back()) = 0;
        to_delete.pop_back();
    }
}


Batch &Renderer::findBatch(std::array<GLuint, N_MAX_TEXTURES> texture_ids, Shader &shader, GLenum draw_type, int num_vertices_inserted)
{
   BatchConfig config = {texture_ids, shader.getId(), draw_type};
    if (m_config2batches.count(config) != 0) //! batch with config exists
    {
        return findFreeBatch(config, shader, draw_type, num_vertices_inserted);
    }

    m_config2batches[config];
    //! create the new batch
    // m_config2next_free_batch[config] = m_config2batches.at(config).size();
    m_config2batches.at(config).push_back(createBatch(config, shader, draw_type));
    return *m_config2batches.at(config).back();
}

SpriteBatch &Renderer::findSpriteBatch(std::array<GLuint, N_MAX_TEXTURES> texture_ids, Shader &shader, GLenum draw_type)
{
    BatchConfig config = {texture_ids, shader.getId(), draw_type};
    if (m_config2sprite_batches.count(config) != 0) //! batch with config exists
    {
        return findFreeSpriteBatch(config, shader, draw_type);
    }

    //! add new configuration
    m_config2sprite_batches[config];
    //! create the new batch
    m_config2sprite_batches.at(config).push_back(std::make_unique<SpriteBatch>(config, shader));
    return *m_config2sprite_batches.at(config).back();
}

Batch &Renderer::findBatch(GLuint texture_id, Shader &shader, GLenum draw_type, int num_vertices_inserted)
{
    BatchConfig config = {texture_id, shader.getId(), draw_type};
    if (m_config2batches.count(config) != 0) //! batch with config exists
    {
        return findFreeBatch(config, shader, draw_type, num_vertices_inserted);
    }

    m_config2batches[config];
    //! create the new batch
    // m_config2next_free_batch[config] = m_config2batches.at(config).size();
    m_config2batches.at(config).push_back(createBatch(config, shader, draw_type));
    return *m_config2batches.at(config).back();
}

SpriteBatch &Renderer::findSpriteBatch(GLuint texture_id, Shader &shader, GLenum draw_type)
{
    BatchConfig config = {texture_id, shader.getId(), draw_type};
    if (m_config2sprite_batches.count(config) != 0) //! batch with config exists
    {
        return findFreeSpriteBatch(config, shader, draw_type);
    }

    //! add new configuration
    m_config2sprite_batches[config];
    //! create the new batch
    m_config2sprite_batches.at(config).push_back(std::make_unique<SpriteBatch>(config, shader));
    return *m_config2sprite_batches.at(config).back();
}

Renderer::BatchPtr Renderer::createBatch(const BatchConfig &config, Shader &shader, GLenum draw_type)
{
    return std::make_unique<Batch>(config, shader, draw_type);
}