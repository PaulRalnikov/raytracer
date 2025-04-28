#pragma once
#include "primitives/ellipsoid.hpp"
#include "utils/random.hpp"

glm::vec3 ssample(const Ellipsoid& ellipsoid, glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng);
float ppdf(const Ellipsoid& ellipsoid, const Ray &ray, float ray_length);
