
#pragma once
#include <string>
#include <iostream>
#include <fstream>

#include "utils/my_glm.hpp"
#include "primitives/material.hpp"

enum PrimitiveType
{
    PLANE,
    ELLIPSOID,
};

struct Primitive
{
    PrimitiveType type;
    glm::vec3 position;
    glm::vec3 geom; // depends on type: plane normal, ellipsoid radiuses or box sizes
    my_quat rotation;
    glm::vec3 color;
    glm::vec3 emission;
    MaterialType material = DIFFUSE;
    float ior; //only for diellectric materials

    glm::vec3 get_normal(glm::vec3 point) const;
    glm::vec3 get_unconverted_normal(glm::vec3 point) const;
};

std::ifstream& operator>> (std::ifstream& in, Primitive& primitive);

std::string to_string(PrimitiveType type);

std::ostream &operator<<(std::ostream &out, const Primitive &p);
