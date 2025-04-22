#include "plane.hpp"
#include <glm/glm.hpp>

glm::vec3 Plane::get_unconverted_normal() const {
    return normal;
}

glm::vec3 Plane::get_normal() const {
    return rotation * normal;
}

std::ifstream &operator>>(std::ifstream &in, Plane &plane) {
    in >> plane.normal;

    std::string command;
    while (in >> command) {
        if (command == "POSITION") {
            in >> plane.position;
        }
        else if (command == "ROTATION") {
            in >> plane.rotation;
        }
        else if (command == "EMISSION") {
            in >> plane.emission;
        }
        else if (command == "COLOR") {
            in >> plane.color;
        }
        else if (command == "METALLIC") {
            plane.material = MaterialType::METALLIC;
        }
        else if (command == "DIELECTRIC") {
            plane.material = MaterialType::DIELECTRIC;
        }
        else if (command == "IOR") {
            in >> plane.ior;
            plane.material = MaterialType::DIELECTRIC;
        } else {
            break;
        }
    }
    return in;

}

std::optional<float> intersect(Ray ray, const Plane &plane) {
    my_quat q_hat = plane.rotation.inverse();

    ray.position = q_hat * (ray.position - plane.position);
    ray.direction = q_hat * ray.direction;

    float result = -glm::dot(ray.position, plane.normal) / glm::dot(ray.direction, plane.normal);
    if (result < 0) {
        return {};
    }
    return result;
}
