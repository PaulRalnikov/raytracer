#pragma once
#include <string>
#include <iostream>
#include <optional>
#include <fstream>

#include "utils/my_glm.hpp"
#include "ray.hpp"
#include "material.hpp"

struct Ellipsoid {
    glm::vec3 position = glm::vec3(0.0);
    glm::vec3 radius = glm::vec3(0.0);
    my_quat rotation = my_quat();
    glm::vec3 color = glm::vec3(0.0);
    glm::vec3 emission = glm::vec3(0.0);
    MaterialType material = MaterialType::DIFFUSE;
    float ior = 0.0; // only for diellectric materials

    glm::vec3 get_normal(glm::vec3 point) const;
    glm::vec3 get_unconverted_normal(glm::vec3 point) const;
};

std::ifstream& operator>> (std::ifstream& in, Ellipsoid& ellipsoid);
std::ostream &operator<<(std::ostream &out, const Ellipsoid &ellipsoid);

std::optional<float> intersect(Ray ray, const Ellipsoid &box);
