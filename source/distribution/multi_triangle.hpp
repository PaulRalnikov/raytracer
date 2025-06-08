#pragma once
#include "distribution/triangle.hpp"
#include "distribution/distribution.hpp"
#include "primitives/triangle.hpp"
#include "bvh/bvh.hpp"

class MultiTriangleDistribution : public IDiirectionDistrudution {
public:
    MultiTriangleDistribution(std::vector<Triangle>&& triangles);

    glm::vec3 sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const final;
    float pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const final;
private:
    BVH m_bvh;
};
