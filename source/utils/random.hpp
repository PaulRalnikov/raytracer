#pragma once
#include <glm/vec3.hpp>

float random_float(float l, float r);

glm::vec3 random_normal_vec3();

//generates random direction over normal
glm::vec3 random_direction(glm::vec3 normal);

glm::vec3 random_cos_weighted(glm::vec3 normal);
