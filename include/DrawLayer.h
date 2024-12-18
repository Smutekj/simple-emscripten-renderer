#pragma once

#include <string>
#include <vector>

#include <FrameBuffer.h>
#include <Renderer.h>

#include "PostEffects.h"

class DrawLayer
{

public:
    DrawLayer(int width, int height);
    DrawLayer(int width, int height, TextureOptions options);

    void toggleActivate();

    bool isActive() const;

    void draw(Renderer &window_rend);

    void drawDirectly(Renderer &canvas);

    void addEffect(std::unique_ptr<PostEffect> effect);

    void setBackground(Color c);
    Color getBackground();

public:
    FrameBuffer m_pixels; //!< pixels of the main canvas
    Renderer m_canvas;    //!< main canvas to draw into

private:
    FrameBuffer m_tmp_pixels1; //!< helper pixels to do post effects
    Renderer m_tmp_canvas1;    //!< helper canvas to do post effects

    FrameBuffer m_tmp_pixels2; //!< helper pixels to do post effects
    Renderer m_tmp_canvas2;    //!< helper canvas to do post effects

    std::vector<std::unique_ptr<PostEffect>> m_effects;

    Color m_background_color = {0, 0, 0, 1};

    bool m_is_active = true;
};

struct LayersHolder
{

    DrawLayer &addLayer(std::string name, int depth, TextureOptions options = {});
    // DrawLayer &addLayerOnTop(std::string name);
    // DrawLayer &addLayerDown(std::string name);
    bool hasLayer(const std::string &name);

    std::shared_ptr<DrawLayer> getLayer(const std::string &name);

    Shader *getShaderP(const std::string &layer, const std::string &shader_id);

    void changeDepth(std::string name, int new_depth);

    void clearAllLayers();

    void setView(View new_view);
    void setView(const std::string &layer_id, View new_view);

    void drawSprite(const std::string &layer, Sprite &sprite, const std::string &shader_id);
    void drawLine(const std::string &layer,
                  utils::Vector2f start, utils::Vector2f end, float thickness, Color color = {0, 1, 0, 1});
    void drawRectangle(const std::string &layer, RectangleSimple &rect, const std::string &shader_id, Color color = {1, 0, 0, 1});

    Renderer &getCanvas(const std::string &name);
    Renderer *getCanvasP(const std::string &name);
    FrameBuffer &getPixels(std::string name);
    void activate(const std::string &name);
    bool isActive(const std::string &name);

    void drawInto(Renderer &target);

    std::map<int, std::shared_ptr<DrawLayer>> m_layers;
    std::map<std::string, int> m_name2depth;
};