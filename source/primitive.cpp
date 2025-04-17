#include "primitive.hpp"

#include <glm/glm.hpp>

#include "utils/my_glm.hpp"

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
        case BOX:
        {
            //hack: abs maximal component of (point / geom) is 1; if set zero to others, we get normal
            glm::vec3 hack = point / geom;
            glm::vec3 abs_hack = glm::abs(hack);
            if (abs_hack.x >= std::max(abs_hack.y, abs_hack.z)) {
                hack.y = 0;
                hack.z = 0;
            }
            else if (abs_hack.y >= std::max(abs_hack.x, abs_hack.z))
            {
                hack.x = 0;
                hack.z = 0;
            }
            else if (abs_hack.z >= std::max(abs_hack.x, abs_hack.y))
            {
                hack.x = 0;
                hack.y = 0;
            }
            normal = glm::normalize(hack);
        }
    }
    return glm::normalize(rotation * normal);
}

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
    out << "Emission: " << p.emission << '\n';
    out << "Material: " << to_string(p.material);
    if (p.material == DIELECTRIC) {
        out << '\n';
        out << "IOR: " << p.ior;
    }
    return out;
}
