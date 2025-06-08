#include "multi_triangle.hpp"

MultiTriangleDistribution::MultiTriangleDistribution(std::vector<Triangle>&& triangles)
{
    m_bvh = BVH(std::move(triangles));
}

glm::vec3 MultiTriangleDistribution::sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const {
    size_t i = random_int(0, m_bvh.size() - 1, rng);
    return ssample(m_bvh[i], point, rng);
}

float MultiTriangleDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    Ray ray(point, direction);
    return m_bvh.pdf(ray);
}
