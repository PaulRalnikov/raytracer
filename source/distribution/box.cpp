#include "box.hpp"

#include <glm/gtc/constants.hpp>

#include "utils/random.hpp"
#include "ray.hpp"

BoxDistribution::BoxDistribution(const Box& a_box) : m_box(a_box) {}

static glm::vec3 surface_point_to_box_coords(glm::vec2 surface_point, int surface_num) {
    if (surface_num == 0)
        return glm::vec3(0.0, surface_point);
    else if (surface_num == 1)
        return glm::vec3(surface_point.x, 0.0, surface_point.y);
    return glm::vec3(surface_point, 0.0);
}

glm::vec3 BoxDistribution::sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const
{
    glm::vec3 box_point;
    do {
        glm::vec3 weights = pairwice_product(m_box.size);
        float side_coin = random_float(0, sum(weights), rng);
        int front_back_coin = random_int(0, 1, rng) * 2 - 1; // 1 or -1

        box_point = random_vec3(-m_box.size, m_box.size, rng);
        if (side_coin <= weights.x) {
            box_point.x = front_back_coin * m_box.size.x;
        }
        else if (side_coin <= weights.x + weights.y) {
            box_point.y = front_back_coin * m_box.size.y;
        }
        else {
            box_point.z = front_back_coin * m_box.size.z;
        }
        box_point = m_box.rotation * box_point + m_box.position;
    } while (glm::length(box_point - point) < 1e-8);

    return glm::normalize(box_point - point);
}

// returns pdf of point distribution
static float get_point_pdf(const Box &box, Ray ray, float ray_legnth)
{
    glm::vec3 intersection_point = ray.position + ray.direction * ray_legnth;
    glm::vec3 normal = box.get_normal(intersection_point);
    glm::vec3 unconverted_normal = box.get_unconverted_normal(intersection_point);

    glm::vec3 pairwice = pairwice_product(box.size);
    float p_y = 1.f / 4.f / glm::pi<float>() / glm::length(unconverted_normal * pairwice);

    return p_y * ray_legnth * ray_legnth / glm::abs(glm::dot(ray.direction, normal));
}

float BoxDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const
{
    Ray ray(point, direction);

    std::optional<float> intersection = intersect(ray, m_box);
    if (!intersection.has_value()) {
        return 0.0;
    }

    float t = intersection.value();
    float result = get_point_pdf(m_box, ray, t);

    static const float SHIFT = 1e-4;
    Ray inner_ray(point + direction * (t + SHIFT), direction);
    intersection = intersect(inner_ray, m_box);
    if (intersection.has_value()) {
        result += get_point_pdf(m_box, ray, intersection.value() + t + SHIFT);
    }

    return result;
}
