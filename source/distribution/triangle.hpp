#pragma once
#include "utils/random.hpp"
#include "primitives/triangle.hpp"

glm::vec3 ssample(const Triangle &triangle, glm::vec3 point, pcg32_random_t &rng);
float ppdf(const Triangle &triangle, const Ray &ray, float ray_legnth);
