#pragma once
#include <glm/glm.hpp>
#include "primitives/triangle.hpp"

struct AABB {
    std::array<glm::vec3, 2> borders;

    AABB();
    explicit AABB(const Triangle &primitive);

    void extend(glm::vec3 point);
    void extend(const AABB& aabb);

    void extend(const Triangle& ellipsoid);

    glm::vec3 size() const;
    glm::vec3 center() const;
    float area() const;
    glm::vec3 operator[](size_t idx) const;
};

std::ostream& operator<< (std::ostream& out, const AABB& aabb);

std::optional<float> intersect(const Ray& ray, const AABB& aabb);
