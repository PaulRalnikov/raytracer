#pragma once
#include <glm/glm.hpp>
#include "node_list.hpp"
#include "types.hpp"

struct Camera {
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
    float fov_x, fov_y;

    static Camera fromGltfNodes(const NodeList &node_list, ConstJsonArray cameras, float aspect_ratio);
};
