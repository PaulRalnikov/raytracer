#include <algorithm>
#include <cmath>
#include <functional>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "distribution/cos_weighted.hpp"
#include "distribution/multi_triangle.hpp"
#include "primitives/triangle.hpp"
#include "scene.hpp"
#include "primitives/ray.hpp"
#include "utils/random.hpp"
#include "task_pool.hpp"

Camera::Camera(
    glm::vec3 a_position,
    glm::vec3 a_right,
    glm::vec3 a_up,
    glm::vec3 a_forward,
    float a_fov_x,
    float a_fov_y
):
    position(a_position),
    right(a_right),
    up(a_up),
    forward(a_forward),
    fov_x(a_fov_x),
    fov_y(a_fov_y)
{}

SceneSettings::SceneSettings(
    size_t a_width,
    size_t a_height,
    glm::vec3 a_backgroud_color,
    size_t a_samples,
    size_t a_max_ray_depth
):
    width(a_width),
    height(a_height),
    background_color(a_backgroud_color),
    samples(a_samples),
    max_ray_depth(a_max_ray_depth)
{}

Scene::Scene(SceneSettings a_settings, std::vector<Triangle> &&a_primitives, Camera a_camera) : m_settings(a_settings),
                                                                                                m_camera(a_camera),
                                                                                                m_bvh(std::move(a_primitives)),
                                                                                                m_mis_distribution()
{
    std::vector<Triangle> light_triangles;
    for (const auto &el : m_bvh) {
        if (el.emission != glm::vec3(0.0)) {
            light_triangles.emplace_back(el);
        }
    }

    m_mis_distribution.add_distribution(std::make_shared<CosWeighttedDistrubution>());
    if (!light_triangles.empty()) {
        m_mis_distribution.add_distribution(
            std::make_shared<MultiTriangleDistribution>(std::move(light_triangles))
        );
    }
}

    std::vector<std::vector<glm::vec3>> Scene::get_pixels()
{
    size_t tasks_count = m_settings.width * m_settings.height;
    std::vector<Task> tasks;
    tasks.reserve(tasks_count);

    std::vector<std::vector<std::future<glm::vec3>>> futures(m_settings.height);

    for (int y = 0; y < m_settings.height; y++) {
        futures[y].reserve(m_settings.width);
        for (int x = 0; x < m_settings.width; x++) {
            tasks.push_back(Task(x, y));
            futures[y].push_back(tasks.back().result.get_future());
        }
    }

    auto pool = TaskPool(std::move(tasks), *this);

    std::vector<std::vector<glm::vec3>> pixels(m_settings.height, std::vector<glm::vec3>(m_settings.width));
    for (int y = 0; y < m_settings.height; y++) {
        for (int x = 0; x < m_settings.width; x++) {
            pixels[y][x] = futures[y][x].get();
        }
    }
    return pixels;
}

glm::vec3 Scene::get_pixel_color(int x, int y, pcg32_random_t& rng) const
{
    glm::vec3 color(0.0);
    for (size_t i = 0; i < m_settings.samples; i++)
    {
        glm::vec2 coords = glm::vec2(x, y) + random_vec2(glm::vec3(0), glm::vec2(1), rng);
        Ray ray = ray_to_pixel(coords);
        color += raytrace(ray, rng);
    }
    color /= m_settings.samples;
    return color;
}

glm::vec3 Scene::raytrace(Ray ray, pcg32_random_t &rng, int depth) const
{
    if (depth >= m_settings.max_ray_depth)
    {
        return glm::vec3(0);
    }
    auto intersection = m_bvh.iintersect(ray);
    if (!intersection.has_value())
        return m_settings.background_color;

    const Triangle &triangle = intersection.value().second;
    glm::vec3 inter = intersection.value().first;
    glm::vec3 point = ray.position + ray.direction * inter.z;

    glm::vec3 normal = triangle.get_interpolated_normal(point.xy());
    static const float SHIFT = 1e-4;

    float normal_direction_cos = glm::dot(normal, ray.direction);
    bool inside = (normal_direction_cos > 0);
    if (inside)
    {
        normal *= -1;
        normal_direction_cos *= -1;
    }
    glm::vec3 reflected_direction = glm::normalize(ray.direction - 2 * normal_direction_cos * normal);
    reflected_direction = glm::normalize(reflected_direction);
    Ray reflected_ray(point + reflected_direction * SHIFT, reflected_direction);

    switch (triangle.material)
    {
    case (MaterialType::DIELECTRIC):
    {
        // Snell's law
        float mu_1 = 1;
        float mu_2 = triangle.ior;

        float cos_theta_1 = -normal_direction_cos;
        if (inside)
        {
            std::swap(mu_1, mu_2);
        }

        float sin_theta_2 = mu_1 / mu_2 * std::sqrt(1 - std::pow(cos_theta_1, 2));
        if (std::abs(sin_theta_2) > 1)
        {
            return triangle.emission + raytrace(reflected_ray, rng, depth + 1);
        }
        float cos_theta_2 = std::sqrt(1 - std::pow(sin_theta_2, 2));

        // Shlick's approx
        float r_0 = std::pow((mu_1 - mu_2) / (mu_1 + mu_2), 2);
        float r = r_0 + (1 - r_0) * std::pow(1 - cos_theta_1, 5);

        if (random_float(0, 1, rng) < r)
        {
            // choose reflection
            return triangle.emission + raytrace(reflected_ray, rng, depth + 1);
        }
        // choose refraction
        glm::vec3 refracted_direction = mu_1 / mu_2 * ray.direction + (cos_theta_1 * mu_1 / mu_2 - cos_theta_2) * normal;
        refracted_direction = glm::normalize(refracted_direction);
        Ray refracted_ray(point + refracted_direction * SHIFT, refracted_direction);

        glm::vec3 refracted_color = raytrace(refracted_ray, rng, depth + 1);
        if (!inside)
        {
            refracted_color *= triangle.color;
        }
        return triangle.emission + refracted_color;
    }
    case (MaterialType::DIFFUSE):
    {
        glm::vec3 w = m_mis_distribution.sample(point, normal, rng);
        float normal_w_cos = glm::dot(w, normal);
        if (normal_w_cos <= 0 || triangle.color == glm::vec3(0.0))
        {
            return triangle.emission;
        }
        float pdf = m_mis_distribution.pdf(point, normal, w);
        if (pdf == 0.f)
        {
            return triangle.emission;
        }
        Ray random_ray = Ray(point + w * SHIFT, w);
        glm::vec3 L_in = raytrace(random_ray, rng, depth + 1);
        glm::vec3 result = triangle.emission + triangle.color / glm::pi<float>() * L_in * normal_w_cos / pdf;
        return result;
    }
    case (MaterialType::METALLIC):
        glm::vec3 result = triangle.emission + raytrace(reflected_ray, rng, depth + 1) * triangle.color;
        return result;
    }
    throw std::runtime_error("Unsupported type of material");
}

Ray Scene::ray_to_pixel(glm::vec2 pixel) const {
    float tan_fov_x_2 = tan(m_camera.fov_x / 2);
    float tan_fov_y_2 = tan_fov_x_2 * m_settings.height / (float)m_settings.width;

    glm::vec3 position(
        (2 * pixel.x / m_settings.width - 1) * tan_fov_x_2,
        (-2 * pixel.y / m_settings.height + 1) * tan_fov_y_2,
        1);

    glm::vec3 direction = glm::normalize(
        position.x * m_camera.right +
        position.y * m_camera.up +
        position.z * m_camera.forward
    );
    return Ray(m_camera.position, direction);
}
