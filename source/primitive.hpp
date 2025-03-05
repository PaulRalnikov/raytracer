
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include "../glm/vec3.hpp"
#include "../glm/gtc/quaternion.hpp"

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
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 color;
};


std::string to_string(PrimitiveType type);

std::ostream &operator<<(std::ostream &out, const glm::vec3 v);

std::ostream &operator<<(std::ostream &out, const glm::quat v);

std::ostream &operator<<(std::ostream &out, const Primitive &p);

std::ifstream &operator>>(std::ifstream &in, glm::vec3 &vec);

std::ifstream &operator>>(std::ifstream &in, glm::quat &q);
