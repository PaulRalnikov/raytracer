#pragma once
#include <glm/glm.hpp>

struct Camera {
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
    float fov_x, fov_y;
};
