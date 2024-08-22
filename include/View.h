#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Vector2.h"

class View
{

public:
    View() = default;

    void setSize(float width, float height);
    template <class VecType>
    void setSize(VecType new_size)
    {
        setSize(new_size.x, new_size.y);
    }

    void setCenter(float cx, float cy);
    template <class VecType>
    void setCenter(VecType new_center)
    {
        setCenter(new_center.x, new_center.y);
    }

    utils::Vector2f getCenter()const;
    utils::Vector2f getScale()const;

    void zoom(float factor);

    glm::mat4 &getMatrix();

private:
    void recomputeMatrix();

private:
    glm::mat4 m_view_matrix; //! initialized as the identity matrix

    float m_center_x = 0;
    float m_center_y = 0;
    float m_width = 2;
    float m_height = 2;

    bool m_needs_recompute = true;

};

