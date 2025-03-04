#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Vertex.h"

//! \class Transform
//! \brief basic class which encapsulates a transformation matrix
//! the matrix is recomputed only when needed because an internal flag remembers when some change happened 
class Transform
{
    
public:
    void setPosition(float x, float y);
    template <class VecType>
    void setPosition(VecType new_pos)
    {
        setPosition(new_pos.x, new_pos.y);
    }
    template <class VecType>
    void setScale(VecType s)
    {
        setScale(s.x, s.y);
    }

    void setScale(float sx, float sy);
    void setRotation(float angle);

    Vec2 getPosition()const
    {
        return {m_translation_x, m_translation_y};
    }
    Vec2 getScale()const
    {
        return {m_scale_x, m_scale_y};
    }
    float getRotation()const
    {
        return m_rotation;
    }

    void rotate(float angle);
    void move(float tx, float ty);
    template <class VecType>
    void move(VecType new_pos)
    {
        move(new_pos.x, new_pos.y);
    }

    void scale(float sx, float sy);
    template <class VecType>
    void scale(VecType new_pos)
    {
        scale(new_pos.x, new_pos.y);
    }

    glm::mat4 &getMatrix();

    void transform(Vec2& pos)
    {
        auto& mat = getMatrix();
        auto new_pos = mat * glm::vec4(pos.x, pos.y, 0, 1);
        pos = {new_pos.x, new_pos.y};
        
    }

private:
    glm::mat4 m_matrix;

    float m_translation_x = 0.f;
    float m_translation_y = 0.f;

    float m_rotation = 0.f;
    float m_scale_x = 1.f;
    float m_scale_y = 1.f;
};
