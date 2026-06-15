#include "View.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtc/type_ptr.hpp>

inline glm::mat4 getMatrix(const View &view)
{
    glm::mat4 view_matrix = glm::mat4(1.0f);
    view_matrix = glm::rotate(view_matrix, glm::radians(-view.getAngle()), glm::vec3(0.f, 0.f, 1.f));
    view_matrix = glm::scale(view_matrix, glm::vec3(2.f / view.getSize().x, 2.f / view.getSize().y, 1));
    view_matrix = glm::translate(view_matrix, glm::vec3(-view.getCenter().x, -view.getCenter().y, 0));
    return view_matrix;
}
