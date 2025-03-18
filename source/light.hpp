#pragma once
#include "../glm/vec3.hpp"
#include <optional>

enum LightType{
    POINT,
    DIRECTED
};

struct Light {
    LightType type;
    glm::vec3 intensivity;
    glm::vec3 geom; //direction for directed light od position for point light
    glm::vec3 attenuation;
};
