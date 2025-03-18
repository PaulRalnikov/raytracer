
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

enum MaterialType
{
    METALLIC,
    DIELECTRIC,
    DIFFUSE
};

struct Primitive
{
    PrimitiveType type;
    glm::vec3 position;
    glm::vec3 geom; // depends on type: plane normal, ellipsoid radiuses or box sizes
    my_quat rotation;
    glm::vec3 color;
    MaterialType material = DIFFUSE;
    float ior; //only for diffuse materials
};


std::string to_string(PrimitiveType type);

std::ostream &operator<<(std::ostream &out, const Primitive &p);
