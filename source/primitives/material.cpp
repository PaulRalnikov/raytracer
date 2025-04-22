#include "material.hpp"

std::string to_string(MaterialType type) {
    switch (type)
    {
    case MaterialType::METALLIC:
        return "Metallic";
    case MaterialType::DIFFUSE:
        return "Diffuse";
    case MaterialType::DIELECTRIC:
        return "Dieliectric";
    }
}
