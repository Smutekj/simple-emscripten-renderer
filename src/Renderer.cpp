#include "Renderer.h"

#include "Batch.h"

#include "Rectangle.h"
#include "Texture.h"
#include "Font.h"
#include "Text.h"
#include "Sprite.h"
#include "CommonShaders.inl"

#include <chrono>
#include <numbers>
#include <codecvt>

#include <SDL2/SDL_mouse.h>

#include <glm/trigonometric.hpp>

Renderer::Renderer(RenderTarget &target)
    : m_target(target),
      m_viewport(0.f, 0.f, 1.f, 1.f)
{
    //! load default shaders
    m_shaders.loadFromCode("VertexArrayDefault", vertex_vertexarray_code, fragment_fullpass_code);
    m_shaders.loadFromCode("SpriteDefault", vertex_sprite_code, fragment_fullpass_texture_code);
    m_shaders.loadFromCode("SpritePass", vertex_sprite_code, fragment_fullpass_texture_code_no_alpha);
    m_shaders.loadFromCode("TextDefault", vertex_sprite_code, fragment_text_code);
    m_shaders.loadFromCode("TextDefault2", vertex_text_code, fragment_text2_code);

    //! register Default Batch Types
    m_batches.registerBatch<utils::Vector2f, SpriteInstance>(makeSpriteBatch);
    m_batches.registerBatch<utils::Vector2f, TextInstance>(makeTextBatch);
    m_batches.registerBatch<Vertex, float>(makeVertexBatch);

    m_view = getDefaultView();
}

utils::Vector2i Renderer::getTargetSize() const
{
    return m_target.getSize();
}

ShaderHolder &Renderer::getShaders()
{
    return m_shaders;
}

bool Renderer::hasShader(std::string id)
{
    return m_shaders.getShaders().count(id) > 0;
}

//! \brief clears the target and resets all batches
void Renderer::clear(Color c)
{
    m_target.clear(c);
}

void Renderer::addShader(std::string id, std::string vertex_path, std::string fragment_path)
{
    m_shaders.load(id, vertex_path, fragment_path);
}

Shader &Renderer::getShader(const std::string &id)
{
    return m_shaders.get(id);
}
Shader *Renderer::getShaderP(const std::string &id)
{
    if (hasShader(id))
    {
        return &m_shaders.get(id);
    }
    return nullptr;
}

utils::Vector2f Renderer::getMouseInWorld()
{
    int mouse_coords[2];

    auto m = glm::inverse(m_view.getMatrix());
    SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);
    glm::vec4 world_coords = m * glm::vec4(
                                     2. * mouse_coords[0] / m_target.getSize().x - 1.,
                                     -2. * mouse_coords[1] / m_target.getSize().y + 1.f, 0, 1);
    return {world_coords.x, world_coords.y};
}

//! \returns the position of the mouse in the screen coordinates (upper left corner is 0,0)
utils::Vector2i Renderer::getMouseInScreen()
{
    int mouse_coords[2];
    SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);
    return {mouse_coords[0], mouse_coords[1]};
}

//! \brief useful when drawing directly on the screen (for instance UI and post processing)
//! \returns the view looking exactly at the canvas
View Renderer::getDefaultView() const
{
    View default_view;
    utils::Vector2f window_size = getTargetSize();
    default_view.setCenter(window_size / 2.f);
    default_view.setSize(window_size);
    return default_view;
}

//! \brief checks if shader with \p shader_id exists
//! \brief if not, tries loading it from shaders directory
//! \param shader_id
//! \returns true if the shader with \p shader_id exists
bool Renderer::checkShader(const std::string &shader_id)
{
    if (m_shaders.contains(shader_id))
    {
        return true;
    }

    try
    {
        m_shaders.load(shader_id, "basicinstanced.vert", shader_id + ".frag");
        return true;
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
    return false;
}
//! \brief draws a \p sprite using shader given by \p shader_id
//! \param sprite       contains textures info and sprite transformations
//! \param shader_id
void Renderer::drawSprite(Sprite &sprite, const std::string &shader_id)
{
    if (checkShader(shader_id))
    {
        drawSpriteUnpacked(sprite.getPosition(), sprite.getScale(), sprite.getRotation(), sprite.m_color,
                           sprite.m_tex_rect, sprite.m_tex_size, sprite.m_texture_handles, shader_id);
    }
}

void Renderer::drawText2(const Text &text, const std::string &shader_id)
{
    if (!checkShader(shader_id))
    {
        return;
    }
    auto &shader = m_shaders.get(shader_id);
    const auto &string = text.getTextW();
    auto font = text.getFont();
    if (!font)
    {
        return;
    }

    BatchConfig config({font->getTexture().getHandle(), font->getCharmapTexId()}, &shader);

    TextInstance glyph;
    utils::Vector2f text_scale = text.getScale();
    auto center_pos = text.getPosition();
    auto line_pos = center_pos;
    utils::Vector2f glyph_pos = center_pos;
    for (std::size_t glyph_ind = 0; glyph_ind < string.size(); ++glyph_ind)
    {
        auto character = font->m_characters.at(string.at(glyph_ind));
        float width = character.size.x * text_scale.x;
        float height = character.size.y * text_scale.y;
        float dy = character.size.y - character.bearing.y;

        glyph_pos.x = line_pos.x + character.bearing.x * text_scale.x + width / 2.f;
        glyph_pos.y = line_pos.y + height / 2.f - dy * text_scale.y;

        glyph.pos = glyph_pos;
        glyph.scale = {width / 2.f, height / 2.f};
        glyph.fill_color = text.getColor();
        glyph.edge_color = text.m_edge_color;
        glyph.glow_color = text.m_glow_color;
        glyph.char_code = font->m_charcode2texcode.at(string.at(glyph_ind));

        m_batches.pushInstance(glyph, config);
        //! pushTextInstance(glyph);

        line_pos.x += (character.advance >> 6) * text_scale.x;
    }

    auto bounding_box = text.getBoundingBox();
    line_pos = {bounding_box.pos_x, bounding_box.pos_y};
    utils::Vector2f text_size = {bounding_box.width, bounding_box.height};
    if (text.m_draw_bounding_box)
    {
        drawLineBatched(text.getPosition(), {text.getPosition().x + text_size.x, text.getPosition().y}, 1.5, {1, 0, 0, 1});
        drawLineBatched(line_pos, {line_pos.x + text_size.x, line_pos.y}, 1.5, {0, 1, 0, 1});
        drawLineBatched({line_pos.x + text_size.x, line_pos.y}, {line_pos.x + text_size.x, line_pos.y + text_size.y}, 1.5, {0, 1, 0, 1});
        drawLineBatched({line_pos.x + text_size.x, line_pos.y + text_size.y}, {line_pos.x, line_pos.y + text_size.y}, 1.5, {0, 1, 0, 1});
        drawLineBatched({line_pos.x, line_pos.y + text_size.y}, line_pos, 1.5, {0, 1, 0, 1});
    }
}

//! \brief draws text using a font in the \p text
//! \param text constains Font info
//! \param shader_id
//! \param draw_type
void Renderer::drawText(const Text &text, const std::string &shader_id)
{
    if (!checkShader(shader_id))
    {
        return;
    }
    const auto &string = text.getTextW();
    auto font = text.getFont();
    if (!font)
    {
        return;
    }
    Sprite glyph_sprite(font->getTexture());

    utils::Vector2f text_scale = text.getScale();
    auto line_pos = text.getPosition();
    if (text.m_is_centered)
    {
        auto bb = text.getBoundingBox();
        auto depth = text.getDepthUnderLine();
        line_pos.x -= bb.width / 2.f;
        line_pos.y -= bb.height / 2.f;
        line_pos.y += depth;
    }
    for (std::size_t glyph_ind = 0; glyph_ind < string.size(); ++glyph_ind)
    {
        auto character = font->m_characters.at(string.at(glyph_ind));
        float width = character.size.x * text_scale.x;
        float height = character.size.y * text_scale.y;
        float dy = character.size.y - character.bearing.y;

        utils::Vector2f glyph_pos = {
            line_pos.x + character.bearing.x * text_scale.x + width / 2.f,
            line_pos.y + height / 2.f - dy * text_scale.y};

        glyph_sprite.m_tex_rect = {character.tex_coords.x, character.tex_coords.y,
                                   character.size.x, character.size.y};

        //! setPosition sets center of the sprite not the corner position. so we must correct for that
        glyph_sprite.setPosition(glyph_pos);
        glyph_sprite.setScale(width / 2., height / 2.);
        glyph_sprite.m_color = text.getColor();
        height = character.bb.height;
        width = character.bb.width;
        // drawLineBatched({glyph_pos.x - width/2.f, glyph_pos.y - height/2.f}, {glyph_pos.x + width/2.f, glyph_pos.y-height/2.f}, 0.5, {0, 1, 0, 1});
        // drawLineBatched({glyph_pos.x + width/2.f, glyph_pos.y -height/2.f}, {glyph_pos.x + width/2.f, glyph_pos.y + height/2.f}, 0.5, {0, 1, 0, 1});
        // drawLineBatched({glyph_pos.x + width/2.f, glyph_pos.y +height/2.f}, {glyph_pos.x - width/2.f, glyph_pos.y + height/2.f}, 0.5, {0, 1, 0, 1});
        // drawLineBatched({glyph_pos.x - width/2.f, glyph_pos.y +height/2.f}, {glyph_pos.x - width/2.f, glyph_pos.y - height/2.f}, 0.5, {0, 1, 0, 1});
        line_pos.x += (character.advance >> 6) * text_scale.x;
        drawSprite(glyph_sprite, shader_id);
    }

    auto bounding_box = text.getBoundingBox();
    line_pos = {bounding_box.pos_x, bounding_box.pos_y};
    utils::Vector2f text_size = {bounding_box.width, bounding_box.height};
    if (text.m_draw_bounding_box)
    {
        drawLineBatched(text.getPosition(), {text.getPosition().x + text_size.x, text.getPosition().y}, 1.5, {1, 0, 0, 1});
        drawLineBatched(line_pos, {line_pos.x + text_size.x, line_pos.y}, 1.5, {0, 1, 0, 1});
        drawLineBatched({line_pos.x + text_size.x, line_pos.y}, {line_pos.x + text_size.x, line_pos.y + text_size.y}, 1.5, {0, 1, 0, 1});
        drawLineBatched({line_pos.x + text_size.x, line_pos.y + text_size.y}, {line_pos.x, line_pos.y + text_size.y}, 1.5, {0, 1, 0, 1});
        drawLineBatched({line_pos.x, line_pos.y + text_size.y}, line_pos, 1.5, {0, 1, 0, 1});
    }
}

//! \brief draws Sprite defined by:
//! \param center coordinate of the center
//! \param scale scaling factor
//! \param angle rotation in radians
//! \param color color as 4 0-255 unsigned chars
//! \param tex_rect texture rectangle
//! \param shader_id
//! \param draw_type
void Renderer::drawSpriteUnpacked(Vec2 center, Vec2 scale, float angle, ColorByte color, Rect<int> tex_rect,
                                  Vec2 texture_size, TextureArray &texture_handles,
                                  const std::string &shader_id)
{
    auto &shader = m_shaders.get(shader_id);

    SpriteInstance t;
    t.angle = angle;
    t.trans = center;
    t.scale = scale;
    t.color = color;

    //! normalize the texture rectangle to be between [0,1] just as OpenGL likes it
    auto tex_size = texture_size;
    Rect<float> tex_rect_norm = {tex_rect.pos_x / (float)tex_size.x, tex_rect.pos_y / (float)tex_size.y,
                                 tex_rect.width / (float)tex_size.x, tex_rect.height / (float)tex_size.y};

    //! THE 1- thing is there because all texture atlases have 0,0 in the upper left corner. OpenGL does it in lower left.
    t.tex_coords = {tex_rect_norm.pos_x, 1. - tex_rect_norm.pos_y};
    t.tex_size = {tex_rect_norm.width, tex_rect_norm.height};

    BatchConfig config(texture_handles, &shader);

    m_batches.pushInstance(t, config);
}

//! \brief draws line connecting \p point_a and \p point_b
//! \brief does not batch the call, but instead draws directly
//! \param point_a
//! \param point_b
//! \param thickness
//! \param color
//! TODO: connect with drawLineBatched
void Renderer::drawLine(Vec2 point_a, Vec2 point_b, float thickness, Color color)
{
    DrawRectangle r(m_shaders.get("ShapeDefault"));
    Vec2 dr = {point_b.x - point_a.x, point_b.y - point_a.y};
    r.setRotation(std::atan2(dr.y, dr.x));
    r.setScale(thickness, std::sqrt(dr.x * dr.x + dr.y * dr.y) / 2.f);
    r.setPosition((point_a.x + point_b.x) / 2.f, (point_a.y + point_b.y) / 2.f);
    r.setColor(color);

    m_target.bind();
    r.draw(0, m_view);
}

//! \brief draws line connecting \p point_a and \p point_b
//! \brief batches the call
//! \param point_a
//! \param point_b
//! \param thickness
//! \param color
//! TODO: connect with drawLineBatched
void Renderer::drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color)
{
    if (!checkShader("VertexArrayDefault"))
    {
        return;
    }
    static std::vector<Vertex> verts(6); //! this should be static?
    verts[0] = {{-1.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f}};
    verts[1] = {{1.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 0.f}};
    verts[2] = {{1.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f}};
    verts[3] = {{-1.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 1.f}};
    verts[4] = {{-1.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {0.f, 0.f}};
    verts[5] = {{1.f, 1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f}};

    Vec2 dr = {point_b.x - point_a.x, point_b.y - point_a.y};
    Transform matrix;
    matrix.setRotation(glm::degrees(std::atan2(dr.y, dr.x)));
    matrix.setScale(std::sqrt(dr.x * dr.x + dr.y * dr.y) / 2.f, thickness / 2.f);
    matrix.setPosition((point_a.x + point_b.x) / 2.f, (point_a.y + point_b.y) / 2.f);

    //! set color and transform positions
    for (auto &v : verts)
    {
        matrix.transform(v.pos);
        v.color = color;
    }

    drawVertices(verts);

    // batch.pushVertexArray(verts);
}

//! \brief draws a rectangle \p rect
//! \param rect
//! \param color
//! \param shader_id
//! \param draw_type
void Renderer::drawRectangle(RectangleSimple &rect,
                             const std::string &shader_id)
{

    auto &shader_name = m_shaders.contains(shader_id) ? shader_id : "VertexArrayDefault";
    auto &shader = m_shaders.get(shader_name);

    BatchConfig config({0, 0}, &shader);

    std::vector<Vertex> verts(6); //! this should be static?
    verts[0] = {{-1.f / 2.f, -1.f / 2.f}, rect.m_color, {0.f, 0.f}};
    verts[1] = {{+1.f / 2.f, -1.f / 2.f}, rect.m_color, {1.f, 0.f}};
    verts[2] = {{+1.f / 2.f, +1.f / 2.f}, rect.m_color, {1.f, 1.f}};
    verts[3] = {{-1.f / 2.f, +1.f / 2.f}, rect.m_color, {0.f, 1.f}};
    verts[4] = {{-1.f / 2.f, -1.f / 2.f}, rect.m_color, {0.f, 0.f}};
    verts[5] = {{+1.f / 2.f, +1.f / 2.f}, rect.m_color, {1.f, 1.f}};

    //! set color and transform positions
    for (auto &v : verts)
    {
        rect.transform(v.pos);
    }

    m_batches.pushVertices(verts, config);
}

//! \brief draws a circle centered at: \p center with a radius of \p radius
//! \param center
//! \param radius
//! \param color
//! \param n_verts      number of vertices making the circle
void Renderer::drawCricleBatched(Vec2 center, float radius, Color color, int n_verts)
{
    drawEllipseBatched(center, 0., utils::Vector2f{radius, radius}, color, n_verts);
}

//! \brief draws an ellipse centered at: \p center with a principal radii given by \p scale
//! \param center
//! \param angle        angle of rotation
//! \param scale        principal radii of the ellipse
//! \param color
//! \param n_verts      number of vertices making the circle
void Renderer::drawEllipseBatched(Vec2 center, float angle, const utils::Vector2f &scale, Color color, int n_verts, std::string shader_id)
{
    auto pi = std::numbers::pi_v<float>;

    if (!m_shaders.contains(shader_id))
    {
        return;
    }
    auto &shader = m_shaders.get(shader_id);
    BatchConfig config({0, 0}, &shader);

    auto n_verts_circumference = n_verts - 1;
    std::vector<Vertex> verts;

    float angle_r = glm::radians(angle);
    float d_angle = 2.f * pi / n_verts_circumference;

    utils::Vector2f pos = {std::cos(angle_r), std::sin(angle_r)};
    Vertex v_prev = {{center.x + pos.x * scale.x, center.y + pos.y * scale.y}, color, {pos.x, pos.y}};
    for (int i = 0; i < n_verts_circumference; ++i)
    {
        pos = {std::cos((i + 1) * d_angle + angle_r), std::sin((i + 1) * d_angle + angle_r)};
        Vertex v = {{center.x + pos.x * scale.x, center.y + pos.y * scale.y}, color, {pos.x, pos.y}};

        verts.push_back({center, color, {0, 0}});
        verts.push_back(v_prev);
        verts.push_back(v);

        v_prev = v;
    }

    m_batches.pushVertices(verts, config);
}

//! \brief draws vertices in the \p verts VertexArray using the texture \p p_texture
//! \param verts
//! \param draw_type
//! \param shader_id
//! \param p_texture    pointer to a used texture
void Renderer::drawVertices(std::vector<Vertex> &verts, const std::string &shader_id,
                            std::shared_ptr<Texture> p_texture)
{
    if (!m_shaders.contains(shader_id))
    {
        return;
    }
    auto &shader = m_shaders.get(shader_id);

    GLuint texture_id = p_texture ? p_texture->getHandle() : 0;

    BatchConfig config({texture_id, 0}, &shader);
    m_batches.pushVertices(verts, config);

    // auto &batch = findBatch(texture_id, shader, static_cast<int>(verts.size()));
    // auto n_verts = verts.size();
    // for (size_t i = 0; i < n_verts; ++i)
    // {
    //     batch.pushVertex(verts[i]);
    // }
}


//! \brief sets base directory used for finding shader files
//! \param directory
//! \returns true if directory exists, otherwise returns false
bool Renderer::setShadersPath(std::filesystem::path directory)
{
    return m_shaders.setBaseDirectory(directory);
}

//! \brief deletes batches --
void Renderer::resetBatches()
{
}

void Renderer::drawAllInto(RenderTarget &target)
{
    target.bind();

    if (m_blend_factors_changed)
    {
        setBlendParams(m_blend_factors);
    }

    //! set proper view
    glViewport(m_viewport.pos_x * m_target.getSize().x,
               m_viewport.pos_y * m_target.getSize().y,
               m_viewport.width * m_target.getSize().x,
               m_viewport.height * m_target.getSize().y);

    m_batches.renderAll(m_view);
}

//! \brief draws all the batched calls into a associated RenderTarget
//! \brief THIS NEEDS TO BE CALLED IN ORDER TO SEE ANYTHING ON THE SCREEN
//! \brief (the previous message is for when I forget to do that)
void Renderer::drawAll()
{
    drawAllInto(m_target);
}

RenderTarget &Renderer::getTarget() const
{
    return m_target;
}

void renderToTarget(RenderTarget &target, const Texture &source, Shader &shader)
{

    DrawSprite screen_sprite;
    Vec2 target_size = target.getSize();
    screen_sprite.setPosition(target_size / 2.f);
    screen_sprite.setScale(target_size / 2.f);

    View view;
    view.setCenter(target_size / 2.f);
    view.setSize(target_size);

    screen_sprite.draw(target, shader, {source.getHandle(), 0}, view);
}

void renderToTraget(Renderer &target, const Texture &source, const std::string &shader_id)
{
    if (!target.hasShader(shader_id))
    {
        return;
    }

    auto old_view = target.m_view;
    target.m_view = target.getDefaultView();

    utils::Vector2f target_size = target.getTargetSize();
    Sprite screen_sprite(source);
    screen_sprite.setPosition(target_size / 2.f);
    screen_sprite.setScale(target_size / 2.f);

    target.drawSprite(screen_sprite, shader_id);
    target.drawAll();

    target.m_view = old_view;
}

