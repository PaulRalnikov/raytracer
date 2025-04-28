#include "box.hpp"

#include <glm/gtc/constants.hpp>

#include "utils/random.hpp"
#include "ray.hpp"

// returns pdf of point distribution
static float get_point_pdf(const Box &box, Ray ray, float ray_legnth)
{
    glm::vec3 intersection_point = ray.position + ray.direction * ray_legnth;
    glm::vec3 normal = box.get_normal(intersection_point);

    float p_y = 1.f / 8.f / sum(pairwice_product(box.size));

    return p_y * ray_legnth * ray_legnth / glm::abs(glm::dot(ray.direction, normal));
}

glm::vec3 ssample(const Box &box, glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) {
    glm::vec3 box_point;
    do {
        glm::vec3 weights = pairwice_product(box.size);
        float side_coin = random_float(0, sum(weights), rng);
        int front_back_coin = random_int(0, 1, rng) * 2 - 1; // 1 or -1

        box_point = random_vec3(-box.size, box.size, rng);
        if (side_coin <= weights.x) {
            box_point.x = front_back_coin * box.size.x;
        }
        else if (side_coin <= weights.x + weights.y) {
            box_point.y = front_back_coin * box.size.y;
        }
        else {
            box_point.z = front_back_coin * box.size.z;
        }
        box_point = box.rotation * box_point + box.position;
    } while (glm::length(box_point - point) < 1e-8);

    return glm::normalize(box_point - point);
}

float ppdf(const Box &box, glm::vec3 point, glm::vec3 normal, glm::vec3 direction) {
    Ray ray(point, direction);

    std::optional<float> intersection = intersect(ray, box);
    if (!intersection.has_value()) {
        return 0.0;
    }

    float t = intersection.value();
    float result = get_point_pdf(box, ray, t);

    static const float SHIFT = 1e-4;
    Ray inner_ray(point + direction * (t + SHIFT), direction);
    intersection = intersect(inner_ray, box);
    if (intersection.has_value()) {
        result += get_point_pdf(box, ray, intersection.value() + t + SHIFT);
    }

    return result;
}
