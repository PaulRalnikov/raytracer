#include "ray.hpp"
#include <cmath>
#include "../glm/glm.hpp"

Ray::Ray(glm::vec3 pos, glm::vec3 dir):
    position(pos), direction(dir)
{}

std::ostream &operator<<(std::ostream &out, const Ray &ray) {
    out << "position: " << ray.position << "; " << "direction: " << ray.direction;
    return out;
}

std::optional<float> intersect_ray_with_primitive (Ray ray, Primitive primitive) {
    const float EPS = 1e-6;
    switch (primitive.type)
    {
        case PLANE: {
            glm::vec3 real_normal = glm::normalize(primitive.geom * primitive.rotation);
            float result = -glm::dot(ray.position, real_normal) / glm::dot(ray.direction, real_normal);
            if (result < 0)
                return {};
            return result;
        }
        case ELLIPSOID: {
            glm::vec3 real_radius = primitive.geom * primitive.rotation;

            glm::vec3 pos_radius = (ray.position - primitive.position) / real_radius;
            glm::vec3 dir_radius = ray.direction / real_radius;
            //quadratic equation solving
            float A = glm::dot(dir_radius, dir_radius);
            float B = 2 * glm::dot(pos_radius, dir_radius);
            float C = glm::dot(pos_radius, pos_radius) - 1;
            // std::cout << "ray: " << ray << std::endl;
            // std::cout << "dir_radius: " << dir_radius << std::endl;
            std::cout << A << ' ' << B << ' ' << C << std::endl;
            std::cout << std::endl;

            float D = B * B - 4 * A * C;
            if (D < 0) {
                return {};
            }

            D = sqrt(D);
            float t1 = (-B - D) / 2 / A;
            float t2 = (-B + D) / 2 / A;

            if (t1 > 0 && t2 > 0)
                return std::min(t1, t2);
            if (t1 < 0 && t2 < 0)
                return {};

            return std::max(t1, t2);
        }
            return {};
    };
    return {};
}
