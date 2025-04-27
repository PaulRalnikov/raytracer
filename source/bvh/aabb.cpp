#include "aabb.hpp"

AABB::AABB():
    borders{
        glm::vec3(std::numeric_limits<float>::infinity()),
        glm::vec3(-std::numeric_limits<float>::infinity())
    }
{}

AABB::AABB(const Primitive &primitive) : AABB() {
    extend(primitive);
}

void AABB::extend(glm::vec3 point) {
    borders[0] = glm::min(borders[0], point);
    borders[1] = glm::max(borders[1], point);
}

void AABB::extend(const AABB& aabb) {
    borders[0] = glm::min(borders[0], aabb.borders[0]);
    borders[1] = glm::max(borders[1], aabb.borders[1]);
}

void AABB::extend(const Box& box) {
    for (size_t i = 0; i < 8; i++) {
        extend(box[i]);
    }
}

void AABB::extend(const Ellipsoid& ellipsoid) {
    Box box;
    box.size = ellipsoid.radius;
    box.position = ellipsoid.position;
    box.rotation = ellipsoid.rotation;
    extend(box);
}

void AABB::extend(const Triangle& triangle) {
    for (int i = 0; i < 3; i++) {
        extend(triangle.position + triangle.rotation * triangle.coords[i]);
    }
}

void AABB::extend(const Plane& plane) {
    throw std::runtime_error("Can not add plane to AABB");
}

void AABB::extend(const Primitive& primitive) {
    std::visit([this](const auto& prim) {extend(prim);}, primitive);
}

glm::vec3 AABB::operator[](size_t idx) const {
    assert(idx < 8);
    return glm::vec3(
        borders[(idx >> 2) & 1].x,
        borders[(idx >> 1) & 1].y,
        borders[idx & 1].z
    );
}

glm::vec3 AABB::size() const {
    return borders[1] - borders[0];
}

glm::vec3 AABB::center() const {
    return borders[0] + 0.5f * (borders[1] - borders[0]);
}

float AABB::area() const {
    return sum(pairwice_product(size())) * 2;
}

std::ostream &operator<<(std::ostream &out, const AABB &aabb) {
    out << "min: " << aabb.borders[0] << '\n';
    out << "max: " << aabb.borders[1] << '\n';
    return out;
}
