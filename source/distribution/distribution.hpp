#pragma once
#include <glm/glm.hpp>
#include "utils/random.hpp"

class IDiirectionDistrudution{
public:
    virtual ~IDiirectionDistrudution() = default;

    virtual glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const = 0;
    virtual float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const = 0;
};
