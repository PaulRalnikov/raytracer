#pragma once
#include <limits>
#include <vector>
#include <variant>

#include <glm/vec2.hpp>

#include "primitives/primitive.hpp"
#include "ray.hpp"
#include "distribution/mix.hpp"
#include "bvh.hpp"

class Scene {
public:
    void readTxt(std::string txt_path);

    std::vector<std::vector<glm::vec3> > get_pixels();

    glm::vec3 get_pixel_color(int x, int y, pcg32_random_t& rng) const;

private:
    //returns color of pixels
    glm::vec3 raytrace(Ray ray, pcg32_random_t &rng, int depth = 0) const;

    Ray ray_to_pixel(glm::vec2 pixel) const;

    void setup_distribution();

    int m_width, m_height;
    glm::vec3 m_background_color;
    glm::vec3 m_camera_position;
    glm::vec3 m_camera_right;
    glm::vec3 m_camera_up;
    glm::vec3 m_camera_forward;
    float m_fov_x, m_fov_y;
    int max_ray_depth;

    size_t samples; // rays count per pixel

    BVH bvh;
    MixDistribution mis_distribution;
};

