#pragma once
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

typedef struct {
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

int random_int(int l, int r, pcg32_random_t &rng);
float random_float(float l, float r, pcg32_random_t& rng);

glm::vec2 random_vec2(glm::vec2 l, glm::vec2 r, pcg32_random_t& rng);
glm::vec3 random_vec3(glm::vec3 l, glm::vec3 r, pcg32_random_t& rng);

glm::vec3 random_normal_vec3(pcg32_random_t &rng);
