#pragma once
#include <vector>
#include "../glm/vec2.hpp"
#include "primitive.hpp"
#include "ray.hpp"

struct Scene {
public:
    void readTxt(std::string txt_path);

    Ray ray_to_pixel(glm::vec2 pixel);

    // returns position on the ray and color
    std::optional<std::pair<float, glm::vec3> > intersect(Ray ray, bool print);
    glm::vec3 raytrace(Ray ray, bool print = false);

    int width, height;
    glm::vec3 background_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
    float fov_x, fov_y;
    float tan_fov_x_2;
    float tan_fov_y_2;

    std::vector<Primitive> primitives;
};

