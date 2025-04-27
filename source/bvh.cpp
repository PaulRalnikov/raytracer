#include "bvh.hpp"

BVH::BVH(std::vector<Primitive>&& primitives):
    m_primitives(std::move(primitives))
{
    planes_start = std::stable_partition(m_primitives.begin(), m_primitives.end(), [](const Primitive& v) {
        return !std::holds_alternative<Plane>(v);
    });
}

std::vector<Primitive>::const_iterator BVH::begin() const {
    return m_primitives.begin();
}
std::vector<Primitive>::const_iterator BVH::end() const {
    return m_primitives.end();
}

Intersection BVH::intersect(Ray ray, float max_distance) const {
    Intersection intersection = {};
    for (const Primitive& primitive : m_primitives) {
        std::optional<float> prim_intersect = iintersect(ray, primitive);
        if (!prim_intersect.has_value() || prim_intersect.value() > max_distance) {
            continue;
        }
        if (!intersection.has_value() || intersection.value().first > prim_intersect.value()) {
            intersection = std::make_pair(prim_intersect.value(), std::cref(primitive));
        }
    }
    return intersection;
}
