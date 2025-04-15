#include "cos_weighted.hpp"

#include <algorithm>
#include <glm/gtc/constants.hpp>

#include "utils/random.hpp"

glm::vec3 CosWeighttedDistrubution::sample(glm::vec3 point, glm::vec3 normal) const {
    glm::vec3 v = random_normal_vec3();
    return glm::normalize(v + normal);
}

float CosWeighttedDistrubution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const{
    return std::max(0.f, glm::dot(normal, direction) / glm::pi<float>());
}
