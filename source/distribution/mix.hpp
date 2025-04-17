#pragma once
#include "distribution.hpp"
#include <memory>

class MixDistribution : public IDiirectionDistrudution {
public:
    MixDistribution() = default;

    void add_distribution(std::shared_ptr<IDiirectionDistrudution> distribution);

    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;

private:
    std::vector<std::shared_ptr<IDiirectionDistrudution>> m_distribitions;
};
