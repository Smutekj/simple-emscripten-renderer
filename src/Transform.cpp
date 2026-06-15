#include "Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
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

static glm::mat4 getMatrix(const Transform &transform)
{
    glm::mat4 matrix;
    matrix = glm::translate(glm::mat4(1), glm::vec3(transform.getPosition().x, transform.getPosition().y, 0));
    matrix = glm::rotate(matrix, transform.getRotation(), glm::vec3(0, 0, 1));
    matrix = glm::scale(matrix, glm::vec3(transform.getScale().x, transform.getScale().y, 0));
    return matrix;
}

void Transform::transform(utils::Vector2f &pos)
{
    auto mat = getMatrix(*this);
    auto new_pos = mat * glm::vec4(pos.x, pos.y, 0, 1);
    pos = {new_pos.x, new_pos.y};
}