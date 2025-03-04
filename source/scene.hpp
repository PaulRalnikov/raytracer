#pragma once
#include <vector>
#include "primitive.hpp"

struct Scene {
public:
    void readTxt(std::string txt_path);

    int width, height;
    glm::vec3 background_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
    float camera_fov;

    std::vector<Primitive> primitives;
};

