#pragma once
#include "distribution.hpp"
#include "primitives/box.hpp"

glm::vec3 ssample(const Box& box, glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng);
float ppdf(const Box& box, glm::vec3 point, glm::vec3 normal, glm::vec3 direction);
