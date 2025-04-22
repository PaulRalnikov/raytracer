#pragma once
#include <string>
#include <iostream>
#include <optional>
#include <fstream>

#include "utils/my_glm.hpp"
#include "ray.hpp"
#include "material.hpp"

struct Box{
    glm::vec3 position;
    glm::vec3 size;
    my_quat rotation;
    glm::vec3 color;
    glm::vec3 emission;
    MaterialType material = MaterialType::DIFFUSE;
    float ior; // only for diellectric materials

    glm::vec3 get_normal(glm::vec3 point) const;
    glm::vec3 get_unconverted_normal(glm::vec3 point) const;
};

std::ifstream& operator>> (std::ifstream& in, Box& box);

std::optional<float> intersect(const Ray& ray, const Box& box);
