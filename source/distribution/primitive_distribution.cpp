#include "primitive_distribition.hpp"

#include <glm/gtc/constants.hpp>

#include "utils/random.hpp"
#include "ray.hpp"

PrimitiveDistribution::PrimitiveDistribution(Primitive a_primitive):
    m_primitive(a_primitive) {}

glm::vec3 surface_point_to_box_coords(glm::vec2 surface_point, int surface_num) {
    if (surface_num == 0) return glm::vec3(0.0, surface_point);
    else if (surface_num == 1) return glm::vec3(surface_point.x, 0.0, surface_point.y);
    return glm::vec3(surface_point, 0.0);
}

glm::vec3 PrimitiveDistribution::sample(glm::vec3 point, glm::vec3 normal) const {
    glm::vec3 primitive_point;
    switch (m_primitive.type)
    {
    case PLANE:
        throw std::runtime_error("Can not create sample of distribution on plane");
        break;
    case BOX: {
        glm::vec3 weights = pairwice_product(m_primitive.geom);

        float side_coin = random_float(0, sum(weights));
        int front_back_coin = random_int(0, 1) * 2 - 1; // 1 or -1
        primitive_point = random_vec3(-m_primitive.geom, m_primitive.geom);

        if (side_coin <= weights.x) {
            primitive_point.x = front_back_coin * m_primitive.geom.x;
        } else if (side_coin <= weights.x + weights.y) {
            primitive_point.y = front_back_coin * m_primitive.geom.y;
        } else {
            primitive_point.z = front_back_coin * m_primitive.geom.z;
        }
        break;
    }
    case ELLIPSOID: {
        primitive_point = random_normal_vec3() * m_primitive.geom;
        break;
    }
    default:
        throw std::runtime_error("Unexpected type of primitive");
    }
    primitive_point = m_primitive.rotation * primitive_point + m_primitive.position;

    return glm::normalize(primitive_point - point);
}

//returns pdf of point distribution
float get_point_pdf(const Primitive& primitive, Ray ray, float ray_position) {
    glm::vec3 intersection_point = ray.position + ray.direction + ray_position;
    glm::vec3 primitive_normal = primitive.get_normal(intersection_point);

    float p_y;
    switch (primitive.type)
    {
    case BOX: {
        glm::vec3 pairwice = pairwice_product(primitive.geom);
        p_y = 1.f / 8.f / glm::pi<float>() / glm::length(primitive_normal * pairwice);
        break;
    }
    case ELLIPSOID:
    {
        glm::vec3 pairwice = pairwice_product(primitive.geom);
        p_y = 1.f / 8.f / glm::pi<float>() / glm::length(primitive_normal * pairwice);

        break;
    }
    default:
        throw std::runtime_error("Unsupported type of primitive");
    }
    glm::vec3 y_min_x = intersection_point - ray.position;
    return p_y * glm::dot(y_min_x, y_min_x) / glm::length(ray.direction * primitive_normal);
}

float PrimitiveDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    Ray ray(point, direction);
    std::optional<float> intersection = intersect_ray_with_primitive(ray, m_primitive);
    if (!intersection.has_value()) {
        return 0.0;
    }
    float t = intersection.value();
    float result = get_point_pdf(m_primitive, ray, t);

    static const float SHIFT = 1e-4;
    Ray inner_ray(point + direction * (t + SHIFT), direction);
    intersection = intersect_ray_with_primitive(inner_ray, m_primitive);
    if (intersection.has_value()){
        result += get_point_pdf(m_primitive, inner_ray, intersection.value());
    }
    return result;
}
