#include "box.hpp"
#include <glm/glm.hpp>

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
        } else {
            break;
        }
    }
    return in;
}

glm::vec3 Box::get_normal(glm::vec3 point) const {
    return glm::normalize(rotation * get_unconverted_normal(point));
}

glm::vec3 Box::get_unconverted_normal(glm::vec3 point) const {
    point = rotation.inverse() * (point - position);

    // hack: abs maximal component of (point / geom) is 1; if set zero to others, we get normal
    glm::vec3 hack = point / size;
    glm::vec3 abs_hack = glm::abs(hack);
    if (abs_hack.x >= std::max(abs_hack.y, abs_hack.z)) {
        hack.y = 0;
        hack.z = 0;
    }
    else if (abs_hack.y >= std::max(abs_hack.x, abs_hack.z)) {
        hack.x = 0;
        hack.z = 0;
    }
    else if (abs_hack.z >= std::max(abs_hack.x, abs_hack.y)) {
        hack.x = 0;
        hack.y = 0;
    }
    return glm::normalize(hack);
}

static void sort(float& x, float& y) {
    if (x > y) {
        std::swap(x, y);
    }
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
