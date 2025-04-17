#pragma once
#include <limits>
#include <vector>

#include <glm/vec2.hpp>

#include "primitive.hpp"
#include "ray.hpp"
#include "distribution/cos_weighted.hpp"
#include "distribution/primitive_distribition.hpp"
#include "distribution/mix.hpp"

struct Scene {
public:
    void readTxt(std::string txt_path);

    Ray ray_to_pixel(glm::vec2 pixel);

    // returns position on the ray and primitive index
    std::optional<std::pair<float, size_t>> intersect(Ray ray, float max_distance = std::numeric_limits<float>::infinity());
    glm::vec3 raytrace(Ray ray, pcg32_random_t &rng, int depth = 0);

    int width, height;
    glm::vec3 background_color;
    glm::vec3 camera_position;
    glm::vec3 camera_right;
    glm::vec3 camera_up;
    glm::vec3 camera_forward;
    float fov_x, fov_y;
    float tan_fov_x_2;
    float tan_fov_y_2;
    int max_ray_depth;
    size_t samples; //ray per pixel

    std::vector<Primitive> primitives;
    MixDistribution mis_distribution;
};

