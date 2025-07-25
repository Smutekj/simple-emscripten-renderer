#include "Renderer.h"

#include "Rectangle.h"
#include "Texture.h"
#include "Font.h"
#include "CommonShaders.inl"

#include <chrono>
#include <numbers>

BlendParams::BlendParams(BlendFactor src_fact, BlendFactor dst_fact)
    : src_factor(src_fact), dst_factor(dst_fact)
{
}
BlendParams::BlendParams(BlendFactor src_fact, BlendFactor dst_fact, BlendFactor src_a, BlendFactor dst_a)
    : src_factor(src_fact), dst_factor(dst_fact), src_alpha(src_a), dst_alpha(dst_a)
{
}

Renderer::Renderer(RenderTarget &target)
    : m_viewport(0.f, 0.f, 1.f, 1.f),
      m_target(target)
{
    //! load default shaders
    m_shaders.loadFromCode("VertexArrayDefault", vertex_vertexarray_code, fragment_fullpass_code);
    m_shaders.loadFromCode("SpriteDefault", vertex_sprite_code, fragment_fullpass_texture_code);
    m_shaders.loadFromCode("TextDefault", vertex_sprite_code, fragment_text_code);
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
    
    for(auto& [config, batch] : m_config2batches)
    {
        if(config.draw_type != DrawType::Static)
        {
            std::for_each(batch.begin(), batch.end(), [](auto& b){
                b->clear();
            });
        }
    }
    for(auto& [config, batch] : m_config2sprite_batches)
    {
        if(config.draw_type != DrawType::Static)
        {
            std::for_each(batch.begin(), batch.end(), [](auto& b){
                b->clear();
            });
        }
    }
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
    auto window_size = getTargetSize();
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
//! \param draw_type    Static or Dynamic draws
void Renderer::drawSprite(Sprite &sprite, const std::string &shader_id, DrawType draw_type)
{
    if (checkShader(shader_id))
    {
        drawSpriteUnpacked(sprite.getPosition(), sprite.getScale(), sprite.getRotation(), sprite.m_color,
                           sprite.m_tex_rect, sprite.m_tex_size, sprite.m_texture_handles, shader_id, draw_type);
    }
}

//! TODO: Figure out what the idea was here?
void Renderer::drawSpriteDynamic(Sprite &sprite, const std::string &shader_id)
{
    drawSprite(sprite, shader_id, DrawType::Dynamic);
}

//! \brief draws text using a font in the \p text
//! \param text constains Font info
//! \param shader_id
//! \param draw_type
void Renderer::drawText(Text &text, const std::string &shader_id, DrawType draw_type)
{
    if (!checkShader(shader_id))
    {
        return;
    }
    auto &string = text.getText();
    auto font = text.getFont();
    if (!font)
    {
        return;
    }
    Sprite glyph_sprite(font->getTexture());

    auto text_scale = text.getScale();
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

        glyph_sprite.m_tex_rect = {character.tex_coords.x, character.tex_coords.y,
                                   character.size.x, character.size.y};

        //! setPosition sets center of the sprite not the corner position. so we must correct for that
        glyph_sprite.setPosition(glyph_pos);
        glyph_sprite.setScale(width / 2., height / 2.);
        glyph_sprite.m_color = text.getColor();

        line_pos.x += (character.advance >> 6) * text_scale.x;
        drawSprite(glyph_sprite, shader_id, draw_type);
    }

    auto bounding_box = text.getBoundingBox();
    line_pos = {bounding_box.pos_x, bounding_box.pos_y};
    utils::Vector2f text_size = {bounding_box.width, bounding_box.height};
    if (text.m_draw_bounding_box)
    {
        drawLineBatched(line_pos, {line_pos.x + text_size.x, line_pos.y}, 1, {0, 1, 0, 1});
        drawLineBatched({line_pos.x + text_size.x, line_pos.y}, {line_pos.x + text_size.x, line_pos.y + text_size.y}, 1, {0, 1, 0, 1});
        drawLineBatched({line_pos.x + text_size.x, line_pos.y + text_size.y}, {line_pos.x, line_pos.y + text_size.y}, 1, {0, 1, 0, 1});
        drawLineBatched({line_pos.x, line_pos.y + text_size.y}, line_pos, 1, {0, 1, 0, 1});
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
                                  const std::string &shader_id, DrawType draw_type)
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

    auto &batch = findSpriteBatch(texture_handles, shader, draw_type);
    if (batch.addSprite(t))
    {
    }
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
void Renderer::drawLineBatched(Vec2 point_a, Vec2 point_b, float thickness, Color color,
                               DrawType draw_type)
{
    if (!checkShader("VertexArrayDefault"))
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
}

//! \brief draws a rectangle \p rect
//! \param rect
//! \param color
//! \param shader_id
//! \param draw_type
void Renderer::drawRectangle(RectangleSimple &rect, Color color,
                             const std::string &shader_id, DrawType draw_type)
{

    auto &shader_name = m_shaders.contains(shader_id) ? shader_id : "VertexArrayDefault";

    auto &batch = findBatch(0, m_shaders.get(shader_name), draw_type, 6);

    std::vector<Vertex> verts(6); //! this should be static?
    verts[0] = {{-rect.width / 2.f, -rect.height / 2.f}, color, {0.f, 0.f}};
    verts[1] = {{+rect.width / 2.f, -rect.height / 2.f}, color, {1.f, 0.f}};
    verts[2] = {{+rect.width / 2.f, +rect.height / 2.f}, color, {1.f, 1.f}};
    verts[3] = {{-rect.width / 2.f, +rect.height / 2.f}, color, {0.f, 1.f}};
    verts[4] = {{-rect.width / 2.f, -rect.height / 2.f}, color, {0.f, 0.f}};
    verts[5] = {{+rect.width / 2.f, +rect.height / 2.f}, color, {1.f, 1.f}};

    //! set color and transform positions
    for (auto &v : verts)
    {
        rect.transform(v.pos);
    }

    batch.pushVertexArray(verts);
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

//! \brief draws vertices in the \p verts VertexArray using the texture \p p_texture
//! \param verts
//! \param draw_type
//! \param shader_id
//! \param p_texture    pointer to a used texture
void Renderer::drawVertices(VertexArray &verts, const std::string &shader_id,
                            DrawType draw_type, std::shared_ptr<Texture> p_texture)
{

    if (!m_shaders.contains(shader_id))
    {
        return;
    }
    auto &shader = m_shaders.get(shader_id);

    GLuint texture_id = p_texture ? p_texture->getHandle() : 0;
    auto &batch = findBatch(texture_id, shader, draw_type, static_cast<int>(verts.size()));

    auto n_verts = verts.size();

    for (size_t i = 0; i < n_verts; ++i)
    {
        batch.pushVertex(verts[i]);
    }
}

//! \brief does GL calls for setting the blend function parameters
//! \param params   OpenGL blend equation parameters
static void setBlendFunc(BlendParams params = {})
{
    auto df = static_cast<GLuint>(params.dst_factor);
    auto da = static_cast<GLuint>(params.dst_alpha);
    auto sf = static_cast<GLuint>(params.src_factor);
    auto sa = static_cast<GLuint>(params.src_alpha);

    glBlendFuncSeparate(sf, df, sa, da);
    glCheckErrorMsg("Error in glBlendFuncSeparate!");
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
    m_config2sprite_batches.clear();
    m_config2batches.clear();
}

//! \brief draws all the batched calls into a associated RenderTarget
//! \brief THIS NEEDS TO BE CALLED IN ORDER TO SEE ANYTHING ON THE SCREEN
//! \brief (the previous message is for when I forget to do that)
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
        if (config.draw_type != DrawType::Static)
        {
            to_delete.push_back(config);
        }
    }

    while (!to_delete.empty())
    {
        // m_config2sprite_batches.at(to_delete.back()).clear();
        to_delete.pop_back();
    }
    for (auto &[config, batches] : m_config2batches)
    {

        for (auto &batch : batches)
        {
            batch->flush(m_view);
        }

        //! clear dynamic batches
        if (config.draw_type != DrawType::Static)
        {
            to_delete.push_back(config);
        }
    }

    while (!to_delete.empty())
    {
        // m_config2batches.at(to_delete.back()).clear();
        to_delete.pop_back();
    }
}

//! \param texture_ids  An array containing Open GL texture ids
//! \param shader
//! \param draw_type
//! \returns an existing batch with corresponding configuration
//! \returns a new batch in case no batch with the configuration exists
Batch &Renderer::findBatch(TextureArray texture_ids, Shader &shader, DrawType draw_type, int num_vertices_inserted)
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

//! \param texture_ids
//! \param shader
//! \param draw_type
//! \returns an existing batch with corresponding configuration
//! \returns a new batch in case no batch with the configuration exists
SpriteBatch &Renderer::findSpriteBatch(TextureArray texture_ids, Shader &shader, DrawType draw_type)
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

//! \param texture_id
//! \param shader
//! \param draw_type
//! \returns an existing batch with corresponding configuration
//! \returns a new batch in case no batch with the configuration exists
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

//! \param texture_id
//! \param shader
//! \param draw_type
//! \returns an existing batch with corresponding configuration
//! \returns a new batch in case no batch with the configuration exists
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

//! \brief a factory method for a batch
//! \returns a pointer to a batch
Renderer::BatchPtr Renderer::createBatch(const BatchConfig &config, Shader &shader, DrawType draw_type)
{
    return std::move(std::make_unique<Batch>(config, shader, draw_type));
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
    batches.emplace_back(createBatch(config, shader, draw_type));
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
