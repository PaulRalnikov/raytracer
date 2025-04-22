#pragma once
#include "distribution.hpp"
#include "primitive.hpp"

class PrimitiveDistribution : public IDiirectionDistrudution {
public:
    PrimitiveDistribution(Primitive a_primitive = Primitive());

    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;

private:
    Primitive m_primitive;
};
