#include "DrawLayer.h"

#include "Sprite.h"

DrawLayer::DrawLayer(int width, int height) : m_pixels(width, height),
                                              m_canvas(m_pixels),
                                              m_tmp_pixels1(width, height),
                                              m_tmp_canvas1(m_tmp_pixels1),
                                              m_tmp_pixels2(width, height),
                                              m_tmp_canvas2(m_tmp_pixels2)
{
}

DrawLayer::DrawLayer(int width, int height, TextureOptions options, int mult) : m_mult(mult),
                                                                                m_pixels(width, height, options),
                                                                                m_canvas(m_pixels),
                                                                                m_tmp_pixels1(width, height, options),
                                                                                m_tmp_canvas1(m_tmp_pixels1),
                                                                                m_tmp_pixels2(width, height, options),
                                                                                m_tmp_canvas2(m_tmp_pixels2)
{
}

void DrawLayer::resize(int w, int h)
{

    m_pixels.resize(w / m_mult, h / m_mult);
    m_tmp_pixels1.resize(w / m_mult, h / m_mult);
    m_tmp_pixels2.resize(w / m_mult, h / m_mult);

    for (auto &effect : m_effects)
    {
        effect->resize(w / m_mult, h / m_mult);
    }
}

void DrawLayer::toggleActivate()
{
    m_is_active = !m_is_active;
}

bool DrawLayer::isActive() const
{
    return m_is_active;
}

void DrawLayer::draw(Renderer &window_rend)
{
    int n_effects = m_effects.size();
    if (n_effects >= 2)
    {
        assert(n_effects % 2 != 1); //! not working with odd number of effects!
        m_tmp_canvas1.clear({0, 0, 0, 0});
        m_effects.at(0)->process(m_pixels.getTexture(), m_tmp_canvas1);
        for (int i = 1; i < n_effects - 1; ++i)
        {
            auto &source = i % 2 == 1 ? m_tmp_pixels1.getTexture() : m_tmp_pixels2.getTexture();
            auto &target = i % 2 == 1 ? m_tmp_canvas2 : m_tmp_canvas1;
            m_effects.at(i)->process(source, target);
        }

        auto &source = n_effects % 2 == 0 ? m_tmp_pixels1.getTexture() : m_tmp_pixels2.getTexture();
        m_effects.at(n_effects - 1)->process(source, window_rend);
    }
    else if (n_effects == 1)
    {
        m_canvas.drawAll();
        m_effects.at(0)->process(m_pixels.getTexture(), window_rend);
    }
    else if (n_effects == 0)
    {
        m_canvas.drawAllInto(window_rend.getTarget());
    }
}

void DrawLayer::drawDirectly(Renderer &target)
{
    auto old_view = target.m_view;
    auto target_size = target.getTargetSize();
    Sprite screen_sprite(m_pixels.getTexture());
    screen_sprite.setPosition(target_size / 2.f);
    screen_sprite.setScale(target_size / 2.f);

    target.m_view.setCenter(screen_sprite.getPosition());
    target.m_view.setSize(target_size);

    target.drawSprite(screen_sprite, "SpriteDefault");
    target.drawAll();
    target.m_view = old_view;
}

void DrawLayer::addEffect(std::unique_ptr<PostEffect> effect)
{
    m_effects.push_back(std::move(effect));
}

void DrawLayer::setBackground(Color c)
{
    m_background_color = c;
}

Color DrawLayer::getBackground()
{
    return m_background_color;
}

DrawLayer &LayersHolder::addLayer(std::string name, int depth, TextureOptions options, int height, int width, int mult)
{
    auto new_layer = std::make_shared<DrawLayer>(height, width, options, mult);
    m_layers[depth] = new_layer;
    m_name2depth[name] = depth;
    return *new_layer;
}

bool LayersHolder::hasLayer(const std::string &name)
{
    return m_name2depth.count(name) > 0;
}

std::shared_ptr<DrawLayer> LayersHolder::getLayer(const std::string &name)
{
    if (!hasLayer(name))
    {
        return nullptr;
    }
    assert(m_layers.count(m_name2depth.at(name)) > 0);
    return m_layers.at(m_name2depth.at(name));
}

void LayersHolder::activate(const std::string &name)
{
    auto layer = getLayer(name);
    if (layer)
    {
        layer->toggleActivate();
    }
}

bool LayersHolder::isActive(const std::string &name)
{
    auto layer = getLayer(name);
    if (layer)
    {
        return layer->isActive();
    }
    return false;
}

Shader *LayersHolder::getShaderP(const std::string &layer_name, const std::string &shader_id)
{
    auto layer = getLayer(layer_name);
    if (layer)
    {

        auto &canvas = layer->m_canvas;
        if (canvas.hasShader(shader_id))
        {
            return &canvas.getShader(shader_id);
        }
    }
    return nullptr;
}
void LayersHolder::changeDepth(std::string name, int new_depth)
{

    auto layer = getLayer(name);
    if (layer)
    {
        auto old_depth = m_name2depth.at(name);
        if (m_layers.count(new_depth) > 0) //! if depth already exists do nothing
        {
            return;
        }
        //! otherwise remove old_depth and add new depth
        m_layers.erase(old_depth);
        m_name2depth.at(name) = new_depth;
        m_layers[new_depth] = layer;
    }
}

Renderer &LayersHolder::getCanvas(const std::string &name)
{
    return m_layers.at(m_name2depth.at(name))->m_canvas;
}
Renderer *LayersHolder::getCanvasP(const std::string &name)
{
    if (hasLayer(name))
    {
        return &getCanvas(name);
    }
    return nullptr;
}

FrameBuffer &LayersHolder::getPixels(std::string name)
{
    return m_layers.at(m_name2depth.at(name))->m_pixels;
}

void LayersHolder::clearAllLayers()
{
    for (auto &[depth, layer] : m_layers)
    {
        layer->m_pixels.clear(layer->getBackground());
    }
}
void LayersHolder::setView(View new_view)
{
    for (auto &[depth, layer] : m_layers)
    {
        layer->m_canvas.m_view = new_view;
    }
}
void LayersHolder::setView(const std::string &layer_id, View new_view)
{
    if (m_name2depth.count(layer_id) == 0)
    {
        return;
    }
    m_layers.at(m_name2depth.at(layer_id))->m_canvas.m_view = new_view;
}

void LayersHolder::drawInto(Renderer &target)
{
    for (auto &[depth, layer] : m_layers)
    {
        if (layer->isActive())
        {
            layer->draw(target);
        }
    }
}

void LayersHolder::drawSprite(const std::string &layer, Sprite &sprite, const std::string &shader_id)
{
    auto p_canvas = getCanvasP(layer);
    if (p_canvas)
    {
        p_canvas->drawSprite(sprite, shader_id);
    }
}

void LayersHolder::drawLine(const std::string &layer,
                            utils::Vector2f start, utils::Vector2f end, float thickness, Color color)
{
    auto p_canvas = getCanvasP(layer);
    if (p_canvas)
    {
        p_canvas->drawLineBatched(start, end, thickness, color);
    }
}

void LayersHolder::drawRectangle(const std::string &layer, RectangleSimple &rect, const std::string &shader_id, Color color)
{
    auto p_canvas = getCanvasP(layer);
    if (p_canvas)
    {
        p_canvas->drawRectangle(rect, shader_id);
    }
}

void LayersHolder::resize(int w, int h)
{
    for (auto &[d, canvas] : m_layers)
    {
        canvas->resize(w, h);
    }
}