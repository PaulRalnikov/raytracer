#include "ellipsoid.hpp"
#include <glm/glm.hpp>

std::ifstream &operator>>(std::ifstream &in, Ellipsoid &ellipsoid) {
    in >> ellipsoid.size;

    std::string command;
    while (in >> command) {
        if (command == "POSITION"){
            in >> ellipsoid.position;
        }
        else if (command == "ROTATION") {
            in >> ellipsoid.rotation;
        }
        else if (command == "EMISSION") {
            in >> ellipsoid.emission;
        }
        else if (command == "COLOR") {
            in >> ellipsoid.color;
        }
        else if (command == "METALLIC") {
            ellipsoid.material = MaterialType::METALLIC;
        }
        else if (command == "DIELECTRIC") {
            ellipsoid.material = MaterialType::DIELECTRIC;
        }
        else if (command == "IOR") {
            in >> ellipsoid.ior;
            ellipsoid.material = MaterialType::DIELECTRIC;
        } else {
            break;
        }
    }
    return in;
}

std::ostream &operator<<(std::ostream &out, const Ellipsoid &ellipsoid)
{
    out << "Ellipsoid" << '\n';
    out << "position: " << ellipsoid.position << '\n';
    out << "size: " << ellipsoid.size << '\n';
    out << "rotation: " << ellipsoid.rotation << '\n';
    out << "color: " << ellipsoid.color << '\n';
    out << "emission: " << ellipsoid.emission << '\n';
    out << "matetial: " << to_string(ellipsoid.material) << '\n';
    if (ellipsoid.material == MaterialType::DIELECTRIC)
    {
        out << "ior: " << ellipsoid.ior;
    }
    return out;
}

glm::vec3 Ellipsoid::get_normal(glm::vec3 point) const {
    point = rotation.inverse() * (point - position);
    return glm::normalize(rotation * (point / size / size));
}

glm::vec3 Ellipsoid::get_unconverted_normal(glm::vec3 point) const {
    point = rotation.inverse() * (point - position);
    return glm::normalize(point / size);
}

std::optional<float> intersect(Ray ray, const Ellipsoid &ellipsoid) {
    my_quat q_hat = ellipsoid.rotation.inverse();
    ray.position = q_hat * (ray.position - ellipsoid.position);
    ray.direction = q_hat * ray.direction;

    glm::vec3 real_radius = ellipsoid.size;

    glm::vec3 pos_radius = ray.position / real_radius;
    glm::vec3 dir_radius = ray.direction / real_radius;

    //solve quadratic equation At^2 + Bt + C = 0;
    float A = glm::dot(dir_radius, dir_radius);
    float B = 2 * glm::dot(pos_radius, dir_radius);
    float C = glm::dot(pos_radius, pos_radius) - 1;

    float D = B * B - 4 * A * C;
    if (D < 0) {
        return {};
    }

    D = sqrt(D);
    float t1 = (-B - D) / 2 / A;
    float t2 = (-B + D) / 2 / A;

    if (t1 > 0 && t2 > 0) {
        return std::min(t1, t2);
    }
    if (t1 < 0 && t2 < 0) {
        return {};
    }

    return std::max(t1, t2);
}
