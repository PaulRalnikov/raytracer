#pragma once
#include <optional>
#include <iostream>

#include <glm/glm.hpp>

struct Ray {
    explicit Ray(glm::vec3 pos = glm::vec3(), glm::vec3 dir = glm::vec3());

    glm::vec3 position;
    glm::vec3 direction;
};

std::ostream& operator<< (std::ostream& out, const Ray& ray);
