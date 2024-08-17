#include "View.h"

void View::setSize(float width, float height)
{
    m_width = width;
    m_height = height;

    m_needs_recompute = true;
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

cdt::Vector2f View::getCenter() const
{
    return {m_center_x, m_center_y};
}

cdt::Vector2f View::getScale() const
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

void View::recomputeMatrix()
{
    m_view_matrix = glm::mat4(1.0f);
    m_view_matrix = glm::scale(m_view_matrix, glm::vec3(2.f / m_width, 2.f / m_height, 1));
    m_view_matrix = glm::translate(m_view_matrix, glm::vec3(-m_center_x, -m_center_y, 0));

    m_needs_recompute = false;
}
