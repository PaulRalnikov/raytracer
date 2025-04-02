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

    //returns direction from point to light
    glm::vec3 get_direction(glm::vec3 point);
    glm::vec3 get_color(glm::vec3 point);
    float get_distance(glm::vec3 point);
};

