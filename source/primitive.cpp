#include "primitive.hpp"
#include "../glm/glm.hpp"
#include "my_glm.hpp"

std::string to_string(PrimitiveType type)
{
    switch (type)
    {
    case PLANE:
        return "PLANE";
    case ELLIPSOID:
        return "ELLIPSOID";
    case BOX:
        return "BOX";
    default:
        return "UNKNOWN";
    }
}

std::string to_string(MaterialType material) {
    switch (material)
    {
    case METALLIC:
        return "METALLIC";
    case DIELECTRIC:
        return "DIELECTRIC";
    case DIFFUSE:
        return "DIFFUSE";
    default:
        return "UNKNOWN";
    }
}

std::ostream &operator<<(std::ostream &out, const Primitive &p)
{
    out << "Primitive type: " << to_string(p.type) << '\n';
    out << "Position: " << p.position << '\n';
    out << "Geom: " << p.geom << '\n';
    out << "Rotation: " << p.rotation << '\n';
    out << "Color: " << p.color << '\n';
    out << "Material: " << to_string(p.material);
    if (p.material == DIELECTRIC) {
        out << '\n';
        out << "IOR: " << p.ior;
    }
    return out;
}
