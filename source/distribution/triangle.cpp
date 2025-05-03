#include "triangle.hpp"
#include "utils/sort_float.hpp"

// Generates three floats with sum = 1
static inline glm::vec3 generate_affine_coords(pcg32_random_t &rng)
{
    glm::vec2 vec = random_vec2(glm::vec2(0), glm::vec2(1), rng);
    sort(vec.x, vec.y);
    return glm::vec3(vec.x, vec.y - vec.x, 1.f - vec.y);
}

glm::vec3 ssample(const Triangle &triangle, glm::vec3 point, pcg32_random_t &rng) {
    glm::vec3 affine_coords = generate_affine_coords(rng);
    glm::vec3 triangle_point =
        triangle.coords[0] * affine_coords.x +
        triangle.coords[1] * affine_coords.y +
        triangle.coords[2] * affine_coords.z;

    return glm::normalize(triangle_point - point);
}

float ppdf(const Triangle &triangle, const Ray& ray, float ray_legnth) {
    glm::vec3 intersection_point = ray.position + ray.direction * ray_legnth;

    glm::vec3 normal = triangle.get_normal();

    float p_y = 2.f / glm::length(glm::cross(triangle.coords[1] - triangle.coords[0], triangle.coords[2] - triangle.coords[0]));
    return p_y * ray_legnth * ray_legnth / glm::abs(glm::dot(ray.direction, normal));
}
