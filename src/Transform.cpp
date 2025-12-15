#include "Transform.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

void Transform::setPosition(float x, float y)
{
    m_translation_x = x;
    m_translation_y = y;
}

void Transform::setScale(float sx, float sy)
{
    m_scale_x = sx;
    m_scale_y = sy;
}

void Transform::setRotation(float angle)
{
    m_rotation = angle;
}
void Transform::rotate(float angle)
{
    m_rotation += angle;
}

void Transform::move(float tx, float ty)
{
    m_translation_x += tx;
    m_translation_y += ty;
}

void Transform::scale(float sx, float sy)
{
    m_scale_x *= sx;
    m_scale_y *= sy;
}
utils::Vector2f Transform::getPosition() const
{
    return {m_translation_x, m_translation_y};
}
utils::Vector2f Transform::getScale() const
{
    return {m_scale_x, m_scale_y};
}
float Transform::getRotation() const
{
    return m_rotation;
}

glm::mat4 &Transform::getMatrix()
{
    m_matrix = glm::translate(glm::mat4(1), glm::vec3(m_translation_x, m_translation_y, 0));
    m_matrix = glm::rotate(m_matrix, glm::radians(m_rotation), glm::vec3(0, 0, 1));
    m_matrix = glm::scale(m_matrix, glm::vec3(m_scale_x, m_scale_y, 0));
    return m_matrix;
}

void Transform::transform(utils::Vector2f &pos)
{
    auto &mat = getMatrix();
    auto new_pos = mat * glm::vec4(pos.x, pos.y, 0, 1);
    pos = {new_pos.x, new_pos.y};
}