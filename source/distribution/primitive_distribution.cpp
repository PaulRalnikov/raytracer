#include "primitive_distribition.hpp"
#include "utils/random.hpp"

PrimitiveDistribution::PrimitiveDistribution(Primitive& a_primitive):
    m_primitive(a_primitive) {}

glm::vec3 surface_point_to_box_coords(glm::vec2 surface_point, int surface_num) {
    if (surface_num == 0) return glm::vec3(0.0, surface_point);
    else if (surface_num == 1) return glm::vec3(surface_point.x, 0.0, surface_point.y);
    return glm::vec3(surface_point, 0.0);
}

glm::vec3 PrimitiveDistribution::sample(glm::vec3 point, glm::vec3 normal) const {
    switch (m_primitive.type)
    {
    case PLANE:
        throw std::runtime_error("Can not create sample of distribution on plane");
        break;
    case BOX:
        float w_x = 4 * m_primitive.geom.y * m_primitive.geom.z;
        float w_y = 4 * m_primitive.geom.x * m_primitive.geom.z;
        float w_z = 4 * m_primitive.geom.x * m_primitive.geom.y;

        float side_coin = random_float(0, w_x + w_y + w_z);
        int front_back_coin = random_int(0, 1) * 2 - 1; // 1 or -1
        glm::vec3 box_point = random_vec3(-m_primitive.geom, m_primitive.geom);

        if (side_coin <= w_x) {
            box_point.x = front_back_coin * m_primitive.geom.x;
        } else if (side_coin <= w_x + w_y) {
            box_point.y = front_back_coin * m_primitive.geom.y;
        } else {
            box_point.z = front_back_coin * m_primitive.geom.z;
        }

        box_point += m_primitive.position;
        box_point = m_primitive.rotation * box_point;

        return glm::normalize(box_point - point);
    }
    throw std::runtime_error("Unexpected type of primitive");
}

float PrimitiveDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    return 1.f;
}
