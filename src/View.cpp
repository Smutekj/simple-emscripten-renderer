#include "View.h"

#include "ViewMatrix.h"

View::View(const utils::Vector2f &center, const utils::Vector2f &size)
    : m_center_x(center.x), m_center_y(center.y), m_width(size.x), m_height(size.y)
{
    m_needs_recompute = true;
}

void View::setSize(float width, float height)
{
    m_width = width;
    m_height = height;

    m_needs_recompute = true;
}

utils::Vector2f View::getSize() const
{
    return {m_width, m_height};
}

void View::zoom(float factor)
{
    setSize(m_width * factor, m_height * factor);
}

void View::setCenter(float cx, float cy)
{
    m_center_x = cx;
    m_center_y = cy;

    m_needs_recompute = true;
}

utils::Vector2f View::getCenter() const
{
    return {m_center_x, m_center_y};
}

utils::Vector2f View::getScale() const
{
    return {m_width, m_height};
}

void View::setAngle(float angle_deg)
{
    m_angle = angle_deg;
    m_needs_recompute = true;
}
float View::getAngle() const
{
    return m_angle;
}

bool View::contains(const utils::Vector2f &query) const
{

    return query.x >= (m_center_x - m_width / 2.f) && query.x <= (m_center_x + m_width / 2.f) &&
           query.y >= (m_center_y - m_height / 2.f) && query.y <= (m_center_y + m_height / 2.f);
}

//! \brief called automatically whenever the actual matrix is needed
//! \brief and has changed since last time
void View::recomputeMatrix()
{
}

bool View::intersects(const Rectf &query) const
{
    Rectf view_rect = {m_center_x - m_width / 2.f, m_center_y - m_height / 2.f, m_width, m_height};
    return query.intersects(view_rect);
}

utils::Vector2f View::transformToScreen(const utils::Vector2f &world_pos,
                                        const utils::Vector2f &screen_size) const
{
    glm::mat4 view_matrix = getMatrix(*this);
    glm::vec4 screen_pos = view_matrix * glm::vec4(world_pos.x, world_pos.y, 0.f, 1.f);
    return utils::Vector2f{(screen_pos.x + 1.f) / 2.f * screen_size.x, (screen_pos.y + 1.f) / 2.f * screen_size.y};
}

utils::Vector2f View::transformToWorld(const utils::Vector2f &screen_pos,
                                       const utils::Vector2f &screen_size) const
{
    glm::vec4 screen_pos4 = glm::vec4{screen_pos.x / screen_size.x * 2.f - 1.f,
                                      screen_pos.y / screen_size.y * 2.f - 1.f,
                                      0.f, 1.f};
    glm::mat4 view_matrix = glm::inverse(getMatrix(*this));
    glm::vec4 world_pos = view_matrix * glm::vec4(screen_pos.x, screen_pos.y, 0.f, 1.f);
    return utils::Vector2f{world_pos.x, world_pos.y};
}
