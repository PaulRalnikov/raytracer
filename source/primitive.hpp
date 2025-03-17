
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "my_glm.hpp"

enum PrimitiveType
{
    PLANE,
    ELLIPSOID,
    BOX
};

struct Primitive
{
    PrimitiveType type;
    glm::vec3 position;
    glm::vec3 geom; // depends on type
    my_quat rotation;
    glm::vec3 color;
};


std::string to_string(PrimitiveType type);

std::ostream &operator<<(std::ostream &out, const Primitive &p);
