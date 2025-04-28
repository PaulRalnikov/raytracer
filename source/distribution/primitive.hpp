#pragma once
#include "distribution.hpp"
#include "primitives/primitive.hpp"

using FinitePrimitive = std::variant<Box, Ellipsoid, Triangle>;

std::optional<float> intersect(const Ray& ray, const FinitePrimitive& primitive);

class PrimitiveDistribution : public IDiirectionDistrudution {
public:
    PrimitiveDistribution(std::vector<FinitePrimitive>&& primitives);

    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;
private:
    std::vector<FinitePrimitive> m_primitives;
};
