#include "Renderer.h"

#include "Rectangle.h"
#include "Texture.h"
#include "Font.h"

#include <chrono>
#include <numbers>

Renderer::Renderer(RenderTarget &target)
    : m_target(target), m_viewport(0.f, 0.f, 1.f, 1.f)
{
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

void Renderer::clear(Color c)
{
    m_target.clear(c);
}

void Renderer::addShader(std::string id, std::string vertex_path, std::string fragment_path)
{
    m_shaders.load(id, vertex_path, fragment_path);
}

Shader &Renderer::getShader(std::string id)
{
    return m_shaders.get(id);
}

utils::Vector2f Renderer::getMouseInWorld()
{
    int mouse_coords[2];

    auto m = glm::inverse(m_view.getMatrix());
    auto button = SDL_GetMouseState(&mouse_coords[0], &mouse_coords[1]);
    glm::vec4 world_coords = m * glm::vec4(
                                     2. * mouse_coords[0] / m_target.getSize().x - 1.,
                                     -2. * mouse_coords[1] / m_target.getSize().y + 1.f, 0, 1);
    return {world_coords.x, world_coords.y};
}

bool Renderer::checkShader(const std::string &shader_id)
{
    if (m_shaders.contains(shader_id))
    {
        return true;
    }

    try
    {
        std::filesystem::path resources_path = "../Resources/";
        m_shaders.load(shader_id, resources_path.string() + "basicinstanced.vert", resources_path.string() + shader_id + ".frag");
        return true;
    }
    catch (const std::exception &e)
    {
        std::cout << e.what() << "\n";
    }
    return false;
}

void Renderer::drawSprite(Sprite2 &sprite, const std::string& shader_id, DrawType draw_type)
{
    if (checkShader(shader_id))
    {
        drawSpriteUnpacked(sprite.getPosition(), sprite.getScale(), sprite.getRotation(), sprite.m_color,
                   sprite.m_tex_rect, sprite.m_tex_size, sprite.m_texture_handles, shader_id, draw_type);
    }
}
void Renderer::drawSpriteDynamic(Sprite2 &sprite, const std::string& shader_id)
{
    drawSprite(sprite, shader_id, DrawType::Dynamic);
}

void Renderer::drawText(Text &text,const std::string& shader_id, DrawType draw_type)
{
    if (!checkShader(shader_id))
    {
        return;
    }
    auto &string = text.getText();
    auto font = text.getFont();
    Sprite2 glyph_sprite(font->getTexture());

    //! find dimensions of the text
    // auto text_size_x =
    //     std::accumulate(string.begin(), string.end(), 0, [](auto character)
    //                     {
    //     return font->m_characters.at(character).size.x; });
    // auto text_size_y =
    //     *std::max_element(string.begin(), string.end(), [](auto character)
    //                       {
    //     return font->m_characters.at(character).size.y; });

    // utils::Vector2f text_size = {text_size_x, text_size_y};

    auto text_scale = text.getScale();
    auto center_pos = text.getPosition();
    auto upper_left_pos = center_pos;
    utils::Vector2f glyph_pos = center_pos;
    for (std::size_t glyph_ind = 0; glyph_ind < string.size(); ++glyph_ind)
    {
        auto character = font->m_characters.at(string.at(glyph_ind));

        float dy = character.size.y - character.bearing.y;
        glyph_pos.x = upper_left_pos.x + character.bearing.x * text_scale.x;
        glyph_pos.y = upper_left_pos.y + dy * text_scale.y;

        float width = character.size.x * text_scale.x;
        float height = character.size.y * text_scale.y;

        glyph_sprite.m_tex_rect = {character.tex_coords.x, character.tex_coords.y,
                                   character.size.x, character.size.y};

        //! setPosition sets center of the sprite not the corner position. so we must correct for that
        glyph_pos += utils::Vector2f{width, height - 4. * dy} / 2.f; //! the 4 is weird :(
        glyph_sprite.setPosition(glyph_pos);
        glyph_sprite.setScale(width / 2., height / 2.);
        glyph_sprite.m_color = text.getColor();

        upper_left_pos.x += (character.advance >> 6) * text_scale.x;
        drawSprite(glyph_sprite, shader_id, draw_type);
    }
}

void Renderer::drawSpriteUnpacked(Vec2 center, Vec2 scale, float angle, ColorByte color, Rect<int> tex_rect,
                          Vec2 texture_size, std::array<GLuint, N_MAX_TEXTURES> &texture_handles,
                          const std::string& shader_id, DrawType draw_type)
{
    auto &shader = m_shaders.get(shader_id);

    if (draw_type == DrawType::Static)
    {
        std::cout << "STATIC DRAW NOT SUPPORTED YET!!!";
        return;
    }

    Trans t;
    t.angle = angle;
    t.trans = center;
    t.scale = scale;
    t.color = color;

    //! normalize the texture rectangle to be between [0,1] just as OpenGL likes it
    auto tex_size = texture_size;
    Rect<float> tex_rect_norm = {tex_rect.pos_x / tex_size.x, tex_rect.pos_y / tex_size.y,
                                 tex_rect.width / tex_size.x, tex_rect.height / tex_size.y};
    t.tex_coords = {tex_rect_norm.pos_x, tex_rect_norm.pos_y};
    t.tex_size = {tex_rect_norm.width, tex_rect_norm.height};

    auto &batch = findSpriteBatch(texture_handles, shader, DrawType::Dynamic);
    if (batch.addSprite(t))
    {
    }
}

void Renderer::drawSpriteStatic(Vec2 center, Vec2 scale, float angle, Rect<int> tex_rect,
                                Texture &texture, Shader &shader)
{
    auto &batch = findBatch(texture.getHandle(), shader, DrawType::Dynamic, 4);

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
                               DrawType draw_type)
{
    if(!checkShader("VertexArrayDefault"))
    {
        return;
    }
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

void Renderer::drawRectangle(Rectangle2 &r, Color color, const std::string &shader_id, DrawType draw_type)
{

    auto &shader_name = m_shaders.contains(shader_id) ? shader_id : "VertexArrayDefault";

    auto &batch = findBatch(0, m_shaders.get(shader_name), draw_type, 6);

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
    drawEllipseBatched(center, 0., utils::Vector2f{radius, radius}, color, n_verts);
}

void Renderer::drawEllipseBatched(Vec2 center, float angle, const utils::Vector2f& scale, Color color, int n_verts, std::string shader_id)
{
    auto &batch = findBatch(0, m_shaders.get(shader_id), DrawType::Dynamic, n_verts);

    auto pi = std::numbers::pi_v<float>;

    auto n_verts_circumference = n_verts - 1;
    batch.pushVertex({center, color, {0, 0}});
    for (int i = 0; i < n_verts_circumference; ++i)
    {
        float x = std::cos(2.f * i * pi / n_verts_circumference + glm::radians(angle));
        float y = std::sin(2.f * i * pi / n_verts_circumference + glm::radians(angle));
        Vertex v = {{center.x + x * scale.x, center.y + y * scale.y}, color, {x, y}};
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

void Renderer::drawVertices(VertexArray &verts, DrawType draw_type, std::shared_ptr<Texture> p_texture)
{
    GLuint texture_id = p_texture ? p_texture->getHandle() : 0;
    auto &batch = findBatch(texture_id, *verts.m_shader, draw_type, static_cast<int>(verts.size()));

    auto pi = std::numbers::pi_v<float>;
    auto n_verts = verts.size();

    for (int i = 0; i < n_verts; ++i)
    {
        batch.pushVertex(verts[i]);
    }
}

static void setBlendFunc(BlendParams params = {})
{
    auto df = static_cast<GLuint>(params.dst_factor);
    auto da = static_cast<GLuint>(params.dst_alpha);
    auto sf = static_cast<GLuint>(params.src_factor);
    auto sa = static_cast<GLuint>(params.src_alpha);

    glBlendFuncSeparate(sf, df, sa, da);
}

void Renderer::drawAll()
{

    setBlendFunc(m_blend_factors);

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
            batch->flush(m_view);
        }
        //! clear dynamic batches
        if (config.draw_type != DrawType::Dynamic)
        {
            to_delete.push_back(config);
        }
    }

    while (!to_delete.empty())
    {
        to_delete.pop_back();
    }
    for (auto &[config, batches] : m_config2batches)
    {

        for (auto &batch : batches)
        {
            batch->flush(m_view);
        }

        //! clear dynamic batches
        if (config.draw_type != DrawType::Dynamic)
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

Batch &Renderer::findBatch(std::array<GLuint, N_MAX_TEXTURES> texture_ids, Shader &shader, DrawType draw_type, int num_vertices_inserted)
{
    BatchConfig config = {texture_ids, shader.getId(), draw_type};
    if (m_config2batches.count(config) != 0) //! batch with config exists
    {
        return findFreeBatch(config, shader, draw_type, num_vertices_inserted);
    }

    m_config2batches[config];
    //! create the new batch
    m_config2batches.at(config).push_back(createBatch(config, shader, draw_type));
    return *m_config2batches.at(config).back();
}

SpriteBatch &Renderer::findSpriteBatch(std::array<GLuint, N_MAX_TEXTURES> texture_ids, Shader &shader, DrawType draw_type)
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

Batch &Renderer::findBatch(GLuint texture_id, Shader &shader, DrawType draw_type, int num_vertices_inserted)
{
    BatchConfig config = {texture_id, shader.getId(), draw_type};
    if (m_config2batches.count(config) != 0) //! batch with config exists
    {
        return findFreeBatch(config, shader, draw_type, num_vertices_inserted);
    }

    m_config2batches[config];
    //! create the new batch
    m_config2batches.at(config).push_back(createBatch(config, shader, draw_type));
    return *m_config2batches.at(config).back();
}

SpriteBatch &Renderer::findSpriteBatch(GLuint texture_id, Shader &shader, DrawType draw_type)
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

Renderer::BatchPtr Renderer::createBatch(const BatchConfig &config, Shader &shader, DrawType draw_type)
{
    return std::make_unique<Batch>(config, shader, draw_type);
}

Batch &Renderer::findFreeBatch(BatchConfig config, Shader &shader, DrawType draw_type, int num_vertices_inserted)
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
SpriteBatch &Renderer::findFreeSpriteBatch(BatchConfig config, Shader &shader, DrawType draw_type)
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

Text::Text(std::string text)
    : m_text(text)
{
}

void Text::setFont(std::shared_ptr<Font> new_font)
{
    m_font = std::weak_ptr<Font>(new_font);
}
std::shared_ptr<Font> Text::getFont()
{
    return m_font.lock();
}

void Text::setText(const std::string &new_text)
{
    m_text = new_text;
}

const std::string &Text::getText() const
{
    return m_text;
}

void Text::setColor(ColorByte new_color)
{
    m_color = new_color;
}

const ColorByte &Text::getColor() const
{
    return m_color;
}