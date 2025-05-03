#pragma once
#include <string>

enum class MaterialType {
    METALLIC,
    DIELECTRIC,
    DIFFUSE
};

std::string to_string(MaterialType type);
