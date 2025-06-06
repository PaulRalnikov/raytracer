#include "box.hpp"
#include "utils/sort_float.hpp"
#include <glm/glm.hpp>

glm::vec3 Box::get_normal(glm::vec3 point) const
{
    point = rotation.inverse() * (point - position);

    // hack: abs maximal component of (point / geom) is 1; if set zero to others, we get normal
    glm::vec3 hack = point / size;
    glm::vec3 abs_hack = glm::abs(hack);
    if (abs_hack.x >= std::max(abs_hack.y, abs_hack.z))
    {
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
    return glm::normalize(rotation * hack);
}

glm::vec3 Box::operator[](int idx) const {
    assert(idx < 8);
    return position + rotation * glm::vec3(
        size.x * ((int)((idx >> 2) & 1) * 2 - 1),
        size.y * ((int)((idx >> 1) & 1) * 2 - 1),
        size.z * ((int)(idx & 1) * 2 - 1)
    );
}

std::ifstream &operator>>(std::ifstream &in, Box &box) {
    in >> box.size;

    std::string command;
    while (in >> command) {
        if (command == "POSITION"){
            in >> box.position;
        }
        else if (command == "ROTATION") {
            in >> box.rotation;
        }
        else if (command == "EMISSION") {
            in >> box.emission;
        }
        else if (command == "COLOR") {
            in >> box.color;
        }
        else if (command == "METALLIC") {
            box.material = MaterialType::METALLIC;
        }
        else if (command == "DIELECTRIC") {
            box.material = MaterialType::DIELECTRIC;
        }
        else if (command == "IOR") {
            in >> box.ior;
            box.material = MaterialType::DIELECTRIC;
        } else if (command == "NEW_PRIMITIVE") {
            break;
        }
    }
    return in;
}

std::ostream &operator<<(std::ostream &out, const Box &box) {
    out << "Box" << '\n';
    out << "position: " << box.position << '\n';
    out << "size: " << box.size << '\n';
    out << "rotation: " << box.rotation << '\n';
    out << "color: " << box.color << '\n';
    out << "emission: " << box.emission << '\n';
    out << "matetial: " << to_string(box.material) << '\n';
    if (box.material == MaterialType::DIELECTRIC) {
        out << "ior: " << box.ior;
    }
    return out;
}

std::optional<float> intersect(const Ray &ray, const Box &box) {
    my_quat q_hat = box.rotation.inverse();

    Ray rotated_ray(
        q_hat * (ray.position - box.position),
        q_hat * ray.direction
    );

    glm::vec3 t_vec1 = (box.size - rotated_ray.position) / rotated_ray.direction;
    glm::vec3 t_vec2 = (-box.size - rotated_ray.position) / rotated_ray.direction;

    sort(t_vec1.x, t_vec2.x);
    sort(t_vec1.y, t_vec2.y);
    sort(t_vec1.z, t_vec2.z);

    float t1 = vec_max(t_vec1);
    float t2 = vec_min(t_vec2);

    if (t1 > t2 || (t1 < 0 && t2 < 0)) {
        return {};
    }
    if (t1 > 0 && t2 > 0) {
        return std::min(t1, t2);
    }

    return std::max(t1, t2);
}
