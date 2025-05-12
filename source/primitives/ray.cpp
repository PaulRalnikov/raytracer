#include "ray.hpp"

#include <cmath>
#include <glm/glm.hpp>

#include "utils/my_glm.hpp"


#define GLM_FORCE_QUAT_DATA_WXYZ

Ray::Ray(glm::vec3 pos, glm::vec3 dir):
    position(pos), direction(dir)
{}

std::ostream &operator<<(std::ostream &out, const Ray &ray) {
    out << "position: " << ray.position << "; " << "direction: " << ray.direction;
    return out;
}
