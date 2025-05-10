#pragma once
#include <limits>
#include <vector>
#include <variant>

#include <glm/vec2.hpp>

#include "primitives/primitive.hpp"
#include "ray.hpp"
#include "distribution/mix.hpp"
#include "bvh/bvh.hpp"
#include "camera.hpp"

class Scene {
public:
    void readTxt(std::string txt_path);

    static Scene fromGltf(std::string path, int width, int height, int samples);

    std::vector<std::vector<glm::vec3> > get_pixels();

    glm::vec3 get_pixel_color(int x, int y, pcg32_random_t& rng) const;

private:
    //returns color of pixels
    glm::vec3 raytrace(Ray ray, pcg32_random_t &rng, int depth = 0) const;

    Ray ray_to_pixel(glm::vec2 pixel) const;

    void setup_distribution();

    int m_width, m_height;
    glm::vec3 m_background_color;

    int m_max_ray_depth;
    Camera m_camera;
    size_t m_samples; // rays count per pixel

    BVH m_bvh;
    MixDistribution m_mis_distribution;
};

