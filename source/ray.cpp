#include "ray.hpp"
#include <cmath>
#include "my_glm.hpp"
#include "../glm/glm.hpp"
#include "../glm/gtx/quaternion.hpp"
#include "../glm/mat4x4.hpp"
#include "../glm/ext/matrix_transform.hpp"

#define GLM_FORCE_QUAT_DATA_WXYZ

Ray::Ray(glm::vec3 pos, glm::vec3 dir):
    position(pos), direction(dir)
{}

std::ostream &operator<<(std::ostream &out, const Ray &ray) {
    out << "position: " << ray.position << "; " << "direction: " << ray.direction;
    return out;
}

static void sort(float& x, float& y) {
    if (x > y) {
        std::swap(x, y);
    }
}

std::optional<float> intersect_ray_with_primitive (Ray ray, Primitive primitive) {
    my_quat q_hat = primitive.rotation.inverse();
    ray.position -= primitive.position;
    ray.position = q_hat * ray.position;
    ray.direction = q_hat * ray.direction;

    switch (primitive.type)
    {
        case PLANE: {
            glm::vec3 real_normal = glm::normalize(primitive.geom);
            float result = -glm::dot(ray.position, real_normal) / glm::dot(ray.direction, real_normal);
            if (result < 0)
                return {};
            return result;
        }
        case ELLIPSOID: {
            glm::vec3 real_radius = primitive.geom;

            glm::vec3 pos_radius = ray.position / real_radius;
            glm::vec3 dir_radius = ray.direction / real_radius;

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

            if (t1 > 0 && t2 > 0)
                return std::min(t1, t2);
            if (t1 < 0 && t2 < 0)
                return {};

            return std::max(t1, t2);
        }
        case BOX: {
            glm::vec3 t_vec1 = (primitive.geom - ray.position) / ray.direction;
            glm::vec3 t_vec2 = (- primitive.geom - ray.position) / ray.direction;
            sort(t_vec1.x, t_vec2.x);
            sort(t_vec1.y, t_vec2.y);
            sort(t_vec1.z, t_vec2.z);

            float t1 = vec_max(t_vec1);
            float t2 = vec_min(t_vec2);
            if (t1 > t2)
                return {};
            if (t1 > 0 && t2 > 0)
                return std::min(t1, t2);
            if (t1 < 0 && t2 < 0)
                return {};
            return std::max(t1, t2);
        }
        default:
            throw std::runtime_error("Unexpected type of box");
    };
    return {};
}
