#pragma once
#include <vector>
#include "../glm/vec2.hpp"
#include "primitive.hpp"
#include "ray.hpp"
#include "light.hpp"

struct Scene {
public:
    void readTxt(std::string txt_path);

    Ray ray_to_pixel(glm::vec2 pixel);

    // returns position on the ray and primitive index
    std::optional<std::pair<float, size_t> > intersect(Ray ray);
    glm::vec3 raytrace(Ray ray);

    int width, height;
    glm::vec3 background_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
    float fov_x, fov_y;
    float tan_fov_x_2;
    float tan_fov_y_2;
    int ray_depth;
    glm::vec3 abmient;

    std::vector<Primitive> primitives;
    std::vector<Light> lights;
};

