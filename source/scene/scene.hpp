#pragma once
#include <limits>
#include <vector>
#include <variant>

#include <glm/vec2.hpp>

#include "primitives/ray.hpp"
#include "distribution/mix.hpp"
#include "bvh/bvh.hpp"

struct Camera {
    glm::vec3 position;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 forward;
    float fov_x, fov_y;

    Camera(
        glm::vec3 a_position,
        glm::vec3 a_right,
        glm::vec3 a_up,
        glm::vec3 a_forward,
        float a_fov_x,
        float a_fov_y
    );
};

struct SceneSettings{
    size_t width, height;
    glm::vec3 background_color;
    size_t samples;
    size_t max_ray_depth;

    SceneSettings(
        size_t a_width,
        size_t a_height,
        glm::vec3 a_backgroud_color,
        size_t a_samples,
        size_t a_max_ray_depth
    );
};

class Scene {
public:
    Scene(SceneSettings a_settings, std::vector<Triangle> &&a_primitives, Camera a_camera);

    void readTxt(std::string txt_path);

    std::vector<std::vector<glm::vec3> > get_pixels();

    glm::vec3 get_pixel_color(int x, int y, pcg32_random_t& rng) const;

private:
    //returns color of pixels
    glm::vec3 raytrace(Ray ray, pcg32_random_t &rng, int depth = 0) const;

    Ray ray_to_pixel(glm::vec2 pixel) const;

    Camera m_camera;
    SceneSettings m_settings;

    BVH m_bvh;
    MixDistribution m_mis_distribution;
};

