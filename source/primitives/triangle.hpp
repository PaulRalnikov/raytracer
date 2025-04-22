#pragma once
#include <array>
#include <optional>
#include <glm/glm.hpp>

#include "utils/my_glm.hpp"
#include "material.hpp"
#include "ray.hpp"

struct Triangle{
    std::array<glm::vec3, 3> coords = {glm::vec3(0.0), glm::vec3(0.0), glm::vec3(0.0)}  ;

    glm::vec3 position;
    my_quat rotation = my_quat();
    glm::vec3 color = glm::vec3(0.0);
    glm::vec3 emission = glm::vec3(0.0);
    MaterialType material = MaterialType::DIFFUSE;
    float ior = 0.0; // only for diellectric materials

    glm::vec3 get_normal() const;
};

std::ifstream& operator>>(std::ifstream &in, Triangle &triangle);
std::ostream &operator<<(std::ostream &out, const Triangle &triangle);

std::optional<float> intersect(Ray ray, const Triangle &triangle);
