#include "View.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

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

glm::mat4 &View::getMatrix()
{
    if (m_needs_recompute)
    {
        recomputeMatrix();
    }

    return m_view_matrix;
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
    m_view_matrix = glm::mat4(1.0f);
    m_view_matrix = glm::scale(m_view_matrix, glm::vec3(2.f / m_width, 2.f / m_height, 1));
    m_view_matrix = glm::translate(m_view_matrix, glm::vec3(-m_center_x, -m_center_y, 0));

    m_needs_recompute = false;
}

bool View::intersects(const Rectf &query) const
{
    Rectf view_rect = {m_center_x - m_width/2.f, m_center_y - m_height/2.f, m_width, m_height};
    return query.intersects(view_rect);
}
