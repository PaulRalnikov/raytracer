#include "light.hpp"
#include "../glm/glm.hpp"
#include <stdexcept>
#include <limits>

float Light::get_distance(glm::vec3 point) {
    switch (type)
    {
    case (POINT):
        return glm::distance(geom, point);
    case (DIRECTED):
        return std::numeric_limits<float>::infinity();
    default:
        throw std::runtime_error("Undefined light type");
    }
}

glm::vec3 Light::get_direction(glm::vec3 point) {
    switch (type) {
        case (POINT):
            return glm::normalize(geom - point);
        case (DIRECTED):
            return geom;
        default:
            throw std::runtime_error("Undefined light type");
    }
}

glm::vec3 Light::get_color(glm::vec3 point) {
    switch (type)
    {
    case (POINT):
    {
        float r = glm::distance(point, geom);
        float den = attenuation.x + attenuation.y * r + attenuation.z * r * r;
        return intensivity / den;
    }
    case (DIRECTED):
        return intensivity;
    default:
        throw std::runtime_error("Undefined light type");
    }
}
