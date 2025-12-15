#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>

#include "Utils/Vector2.h"

//! \class Transform
//! \brief basic class which encapsulates a transformation matrix
//! the matrix is recomputed only when needed because an internal flag remembers when some change happened 
class Transform
{
    
public:
    void setPosition(float x, float y);
    template <class VecType>
    void setPosition(VecType new_pos);
    
    void setScale(float sx, float sy);
    template <class VecType>
    void setScale(VecType s);
    
    void setRotation(float angle);

    utils::Vector2f getPosition()const;
    utils::Vector2f getScale()const;
    float getRotation()const;
    
    void rotate(float angle);

    void move(float tx, float ty);
    template <class VecType>
    void move(VecType new_pos);

    void scale(float sx, float sy);
    template <class VecType>
    void scale(VecType new_pos);

    glm::mat4 &getMatrix();

    void transform(utils::Vector2f& pos);
private:
    glm::mat4 m_matrix;

    float m_translation_x = 0.f;
    float m_translation_y = 0.f;

    float m_rotation = 0.f;
    float m_scale_x = 1.f;
    float m_scale_y = 1.f;
};

template <class VecType>
void Transform::move(VecType new_pos)
{
    move(new_pos.x, new_pos.y);
}

template <class VecType>
void Transform::scale(VecType new_pos)
{
    scale(new_pos.x, new_pos.y);
}

template <class VecType>
void Transform::setPosition(VecType new_pos)
{
    setPosition(new_pos.x, new_pos.y);
}

template <class VecType>
void Transform::setScale(VecType s)
{
    setScale(s.x, s.y);
}