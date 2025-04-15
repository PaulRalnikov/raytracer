#pragma once
#include <optional>
#include <iostream>

#include "primitive.hpp"

struct Ray
{
    Ray(glm::vec3 pos = glm::vec3(), glm::vec3 dir = glm::vec3());

    glm::vec3 position;
    glm::vec3 direction;
};

std::ostream& operator<< (std::ostream& out, const Ray& ray);

std::optional<float> intersect_ray_with_primitive(Ray ray, Primitive primitive);
