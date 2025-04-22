#include "triangle.hpp"

glm::vec3 Triangle::get_normal() const {
    glm::vec3 ab = coords[1] - coords[0];
    glm::vec3 ac = coords[2] - coords[0];
    return glm::normalize(rotation * glm::cross(ab, ac));
}

std::ifstream &operator>>(std::ifstream &in, Triangle &triangle) {
    for (int i = 0; i < 3; i++) {
        in >> triangle.coords[i];
    }

    std::string command;
    while (in >> command) {
        if (command == "POSITION") {
            in >> triangle.position;
        }
        if (command == "ROTATION") {
            in >> triangle.rotation;
        }
        else if (command == "EMISSION") {
            in >> triangle.emission;
        }
        else if (command == "COLOR") {
            in >> triangle.color;
        }
        else if (command == "METALLIC") {
            triangle.material = MaterialType::METALLIC;
        }
        else if (command == "DIELECTRIC") {
            triangle.material = MaterialType::DIELECTRIC;
        }
        else if (command == "IOR") {
            in >> triangle.ior;
            triangle.material = MaterialType::DIELECTRIC;
        } else if (command == "NEW_PRIMITIVE") {
            break;
        }
    }
    return in;
}

std::ostream &operator<<(std::ostream &out, const Triangle &triangle) {
    out << "triangle" << '\n';
    out << "position: " << triangle.position << '\n';
    out << "coords: " << triangle.coords[0] << ' ' << triangle.coords[1] << ' ' << triangle.coords[2] << '\n';
    out << "rotation: " << triangle.rotation << '\n';
    out << "color: " << triangle.color << '\n';
    out << "emission: " << triangle.emission << '\n';
    out << "matetial: " << to_string(triangle.material) << '\n';
    if (triangle.material == MaterialType::DIELECTRIC) {
        out << "ior: " << triangle.ior;
    }
    return out;
}

std::optional<float> intersect(Ray ray, const Triangle &triangle) {
    my_quat q_hat = triangle.rotation.inverse();

    ray.position = q_hat * (ray.position - triangle.position);
    ray.direction = q_hat * ray.direction;

    //solve LES A + u * (B - A) + v * (C - A) = O + t * D
    // where [A, B, C] = triangle.coords
    // O, D - ray position and direction
    // u, v - <<triangle coords>>, t - ray <<length>>

    //solve Ax = B, where x = [u, v, t]
    glm::mat3x3 A (
        triangle.coords[1] - triangle.coords[0],
        triangle.coords[2] - triangle.coords[0],
        -ray.direction
    );
    glm::vec3 B = ray.position - triangle.coords[0];

    glm::vec3 x = glm::inverse(A) * B;
    float u = x.x, v = x.y, t = x.z;

    if (u >= 0 && v >= 0 && u + v <= 1 && t >= 0) {
        return t;
    }
    return {};
}
