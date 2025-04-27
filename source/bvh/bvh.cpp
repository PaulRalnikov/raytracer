#include "bvh.hpp"
#include <queue>

BVH::BVH(std::vector<Primitive>&& primitives):
    m_primitives(std::move(primitives))
{
    m_planes_end = std::stable_partition(m_primitives.begin(), m_primitives.end(), [](const Primitive& v) {
        return std::holds_alternative<Plane>(v);
    });

    m_root = build(m_planes_end, m_primitives.end());
}

std::vector<Primitive>::const_iterator BVH::begin() const {
    return m_primitives.begin();
}
std::vector<Primitive>::const_iterator BVH::end() const {
    return m_primitives.end();
}

void static inline update_intersection(
    Intersection& intersection,
    std::optional<float> update,
    const Primitive& primitive)
{
    if (update.has_value() && (!intersection.has_value() || update.value() < intersection.value().first)) {
        intersection = std::make_pair(update.value(), std::cref(primitive));
    }
}

static inline Intersection closesed(const Intersection &first, const Intersection &second) {
    return (
        first.has_value() && (!second.has_value() || first.value().first < second.value().first)
        ? first
        : second
    );
}

Intersection BVH::intersect(Ray ray, float max_distance) const
{
    Intersection intersection = {};
    for (auto it = m_primitives.begin(); it < m_planes_end; it++) {
        update_intersection(intersection, iintersect(ray, *it), *it);
    }
    return closesed(intersection, intersect_with_nodes(ray, max_distance));
}

size_t BVH::build(PrimitiveIterator begin, PrimitiveIterator end) {
    m_nodes.push_back(Node(-1, -1, begin, end));
    return m_nodes.size() - 1;
}

Intersection BVH::intersect(
    const Ray& ray,
    const BVH::Node& node,
    float max_distance) const
{
    Intersection intersection = {};
    for (auto it = node.begin; it < node.end; it++) {
        update_intersection(intersection, iintersect(ray, *it), *it);
    }
    return intersection;
}

Intersection BVH::intersect_with_nodes(const Ray& ray, float max_distance) const {
    Intersection result = {};

    std::queue<size_t> queue;
    queue.push(m_root);
    while (!queue.empty()) {
        const Node& node = m_nodes[queue.front()];
        queue.pop();
        result = closesed(result, intersect(ray, node, max_distance));

        if (node.left_child != -1) {
            queue.push(node.left_child);
        }
        if (node.right_child != -1) {
            queue.push(node.right_child);
        }
    }
    return result;
}

BVH::Node::Node(int left_child, int right_child, PrimitiveIterator begin, PrimitiveIterator end):
    left_child(left_child), right_child(right_child), begin(begin), end(end)
{
    for (auto it = begin; it < end; it++) {
        aabb.extend(*it);
    }
    std::cout << "min: " << aabb.borders[0] << "; max:" << aabb.borders[1] << std::endl;
}
