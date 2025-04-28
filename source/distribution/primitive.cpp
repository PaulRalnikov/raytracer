#include "primitive.hpp"
#include "box.hpp"
#include "ellipsoid.hpp"

PrimitiveDistribution::PrimitiveDistribution(std::vector<FinitePrimitive>&& primitives):
    m_primitives(std::move(primitives)) {}

glm::vec3 PrimitiveDistribution::sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const {
    size_t i = random_int(0, m_primitives.size() - 1, rng);
    return std::visit([point, normal, &rng](const auto& primitive) {
        return ssample(primitive, point, normal, rng);
    }, m_primitives[i]);
}

float PrimitiveDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    float sum = 0;
    for (const auto& primitive : m_primitives) {
        std::visit([&sum, point, normal, direction](const auto& prim) {
            sum += ppdf(prim, point, normal, direction);
        }, primitive);
    }
    return sum / m_primitives.size();
}
