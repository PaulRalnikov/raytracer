#pragma once
#include "distribution.hpp"
#include "primitives/ellipsoid.hpp"

glm::vec3 ssample(const Ellipsoid& ellipsoid, glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng);
float ppdf(const Ellipsoid& ellipsoid, glm::vec3 point, glm::vec3 normal, glm::vec3 direction);
