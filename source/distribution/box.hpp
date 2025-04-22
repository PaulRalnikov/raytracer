#pragma once
#include "distribution.hpp"
#include "primitives/box.hpp"

class BoxDistribution : public IDiirectionDistrudution
{
public:
    BoxDistribution(const Box& a_box);

    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;

private:
    Box m_box;
};
