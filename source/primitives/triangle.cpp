#include "triangle.hpp"

glm::vec3 Triangle::get_normal() const {
    glm::vec3 ab = coords[1] - coords[0];
    glm::vec3 ac = coords[2] - coords[0];
    return glm::normalize(glm::cross(ab, ac));
}

std::ifstream &operator>>(std::ifstream &in, Triangle &triangle) {
    for (int i = 0; i < 3; i++) {
        in >> triangle.coords[i];
    }

    glm::vec3 position;
    my_quat rotation;

    std::string command;
    while (in >> command) {
        if (command == "POSITION") {
            in >> position;
        }
        if (command == "ROTATION") {
            in >> rotation;
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
    for (auto& p : triangle.coords) {
        p = position + rotation * p;
    }
    return in;
}

std::ostream &operator<<(std::ostream &out, const Triangle &triangle) {
    out << "triangle" << '\n';
    out << "coords: " << triangle.coords[0] << ' ' << triangle.coords[1] << ' ' << triangle.coords[2] << '\n';
    out << "color: " << triangle.color << '\n';
    out << "emission: " << triangle.emission << '\n';
    out << "matetial: " << to_string(triangle.material) << '\n';
    if (triangle.material == MaterialType::DIELECTRIC) {
        out << "ior: " << triangle.ior;
    }
    return out;
}

std::optional<glm::vec3> intersect(const Ray& ray, const Triangle &triangle) {

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

    if (x.x >= 0 && x.y >= 0 && x.x + x.y <= 1 && x.z >= 0) {
        return x;
    }
    return {};
}
