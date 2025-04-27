#pragma once
#include <glm/glm.hpp>
#include "primitives/primitive.hpp"

struct AABB {
    std::array<glm::vec3, 2> borders;

    AABB();

    void extend(glm::vec3 point);
    void extend(const AABB& aabb);

    void extend(const Box& box);
    void extend(const Ellipsoid& ellipsoid);
    void extend(const Triangle& ellipsoid);
    void extend(const Plane& plane);
    void extend(const Primitive& primitive);

    glm::vec3 operator[](size_t idx) const;
};
