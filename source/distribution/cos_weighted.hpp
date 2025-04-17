#pragma once
#include "distribution.hpp"

class CosWeighttedDistrubution : public IDiirectionDistrudution {
public:
    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;
};
