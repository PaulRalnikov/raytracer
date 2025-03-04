#include "primitive.hpp"

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

std::ostream &operator<<(std::ostream &out, const glm::vec3 v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

std::ostream &operator<<(std::ostream &out, const glm::vec4 v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
}

std::ostream &operator<<(std::ostream &out, const Primitive &p)
{
    out << "Primitive type: " << to_string(p.type) << '\n';
    out << "Position: " << p.position << '\n';
    out << "Geom: " << p.geom << '\n';
    out << "Rotation: " << p.rotation << '\n';
    out << "Color: " << p.color;
    return out;
}

std::ifstream &operator>>(std::ifstream &in, glm::vec3 &vec)
{
    in >> vec.x >> vec.y >> vec.z;
    return in;
}

std::ifstream &operator>>(std::ifstream &in, glm::vec4 &vec)
{
    in >> vec.x >> vec.y >> vec.z >> vec.w;
    return in;
}
