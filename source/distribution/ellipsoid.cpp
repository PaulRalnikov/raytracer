#include "ellipsoid.hpp"

#include <glm/gtc/constants.hpp>

#include "utils/random.hpp"
#include "ray.hpp"

glm::vec3 ssample(const Ellipsoid &ellipsoid, glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) {
    glm::vec3 direction;
    std::optional<float> intersection;
    do {
        glm::vec3 ellipsoid_point = random_normal_vec3(rng) * (ellipsoid.radius);
        ellipsoid_point = ellipsoid.rotation * ellipsoid_point + ellipsoid.position;

        direction = ellipsoid_point - point;
        if (glm::length(direction) < 1e-8)
        {
            continue;
        }
        direction = glm::normalize(direction);
        intersection = intersect(Ray(point, direction), ellipsoid);
    } while (!intersection.has_value());

    return direction;
}

// returns pdf of point distribution
static float get_point_pdf(const Ellipsoid &ellipsoid, Ray ray, float ray_legnth) {
    glm::vec3 intersection_point = ray.position + ray.direction * ray_legnth;

    glm::vec3 normal = ellipsoid.get_normal(intersection_point);
    glm::vec3 unconverted_normal = ellipsoid.get_unconverted_normal(intersection_point);

    glm::vec3 pairwice = pairwice_product(ellipsoid.radius);
    float p_y = 1.f / 4.f / glm::pi<float>() / glm::length(unconverted_normal * pairwice);

    return p_y * ray_legnth * ray_legnth / glm::abs(glm::dot(ray.direction, normal));
}

float ppdf(const Ellipsoid &ellipsoid, glm::vec3 point, glm::vec3 normal, glm::vec3 direction) {
    Ray ray(point, direction);

    std::optional<float> intersection = intersect(ray, ellipsoid);

    if (!intersection.has_value()) {
        return 0.0;
    }
    float t = intersection.value();
    float result = get_point_pdf(ellipsoid, ray, t);

    static const float SHIFT = 1e-4;
    Ray inner_ray(point + direction * (t + SHIFT), direction);
    intersection = intersect(inner_ray, ellipsoid);
    if (intersection.has_value()) {
        result += get_point_pdf(ellipsoid, ray, intersection.value() + t + SHIFT);
    }

    return result;
}
