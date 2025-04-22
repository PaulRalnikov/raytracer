#include "primitive.hpp"

#include <glm/glm.hpp>

#include "utils/my_glm.hpp"

std::ifstream &operator>>(std::ifstream &in, Primitive &primitive) {
    std::string command;
    while (in >> command) {
        if (command == "POSITION") {
            in >> primitive.position;
        }
        else if (command == "ROTATION") {
            in >> primitive.rotation;
        }
        else if (command == "EMISSION") {
            in >> primitive.emission;
        }
        else if (command == "COLOR") {
            in >> primitive.color;
        }
        else if (command == "METALLIC") {
            primitive.material = METALLIC;
        }
        else if (command == "DIELECTRIC") {
            primitive.material = DIELECTRIC;
        }
        else if (command == "IOR") {
            in >> primitive.ior;
            primitive.material = DIELECTRIC;
        } else {
            break;
        }
    }
    return in;
}

glm::vec3 Primitive::get_normal(glm::vec3 point) const {
    point -= position;
    point = rotation.inverse() * point;
    glm::vec3 normal;
    switch (type) {
        case (PLANE):
            normal = geom;
            break;
        case (ELLIPSOID):
            normal = glm::normalize(point / geom / geom);
            break;
    }
    return glm::normalize(rotation * normal);
}

glm::vec3 Primitive::get_unconverted_normal(glm::vec3 point) const
{
    point -= position;
    point = rotation.inverse() * point;
    glm::vec3 normal;
    switch (type)
    {
    case (PLANE):
        normal = geom;
        break;
    case (ELLIPSOID):
        normal = glm::normalize(point / geom);
        break;
    }
    return normal;
}

std::string to_string(PrimitiveType type)
{
    switch (type)
    {
    case PLANE:
        return "PLANE";
    case ELLIPSOID:
        return "ELLIPSOID";
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
    out << "Emission: " << p.emission << '\n';
    out << "Material: " << to_string(p.material);
    if (p.material == DIELECTRIC) {
        out << '\n';
        out << "IOR: " << p.ior;
    }
    return out;
}
