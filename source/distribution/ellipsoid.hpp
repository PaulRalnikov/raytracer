#pragma once
#include "distribution.hpp"
#include "primitives/ellipsoid.hpp"

class EllipsoidDistribution : public IDiirectionDistrudution {
public:
    EllipsoidDistribution(const Ellipsoid &a_ellipsoid);

    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;

private:
    Ellipsoid m_ellipsoid;
};
