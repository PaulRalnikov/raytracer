#pragma once
#include "primitives/box.hpp"
#include "utils/random.hpp"

glm::vec3 ssample(const Box& box, glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng);
float ppdf(const Box& box, const Ray& ray, float ray_length);
