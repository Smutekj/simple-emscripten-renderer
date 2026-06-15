#pragma once

#include "Vector2.h"
#include "Rect.h"

//! \class View
//! \brief defines rectangle in the world coordinates, which will be drawn on screen
//! used to specify, which part of the world to draw
class View
{

public:
    View() = default;
    View(const utils::Vector2f &center, const utils::Vector2f &size);

    void setSize(float width, float height);
    template <class VecType>
    void setSize(VecType new_size);
    utils::Vector2f getSize() const;
    utils::Vector2f getScale() const;

    void setCenter(float cx, float cy);
    template <class VecType>
    void setCenter(VecType new_center);
    utils::Vector2f getCenter() const;

    void setAngle(float angle_deg);
    float getAngle() const;

    void zoom(float factor);

    bool contains(const utils::Vector2f &query) const;
    bool intersects(const Rectf &query) const;

    utils::Vector2f transformToScreen(const utils::Vector2f &world_pos, const utils::Vector2f &screen_size) const;
    utils::Vector2f transformToWorld(const utils::Vector2f &screen_pos, const utils::Vector2f &screen_size) const;

private:
    void recomputeMatrix();

private:
    float m_center_x = 0;
    float m_center_y = 0;
    float m_width = 2;
    float m_height = 2;
    float m_angle = 0.f;

    bool m_needs_recompute = true;
};

template <class VecType>
void View::setSize(VecType new_size)
{
    setSize(new_size.x, new_size.y);
}

template <class VecType>
void View::setCenter(VecType new_center)
{
    setCenter(new_center.x, new_center.y);
}