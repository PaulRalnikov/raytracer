#include "mix.hpp"
#include "utils/random.hpp"

void MixDistribution::add_distribution(std::shared_ptr<IDiirectionDistrudution> distribution) {
    m_distribitions.push_back(distribution);
}

glm::vec3 MixDistribution::sample(glm::vec3 point, glm::vec3 normal) const {
    int idx = random_int(0, (int)m_distribitions.size() - 1);
    return m_distribitions[idx]->sample(point, normal);
}

float MixDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    float sum = 0;
    for (auto el : m_distribitions) {
        sum += el->pdf(point, normal, direction);
    }
    return sum / m_distribitions.size();
}
