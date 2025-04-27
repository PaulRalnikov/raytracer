#include "bvh.hpp"
#include <queue>

BVH::BVH(std::vector<Primitive>&& primitives):
    m_primitives(std::move(primitives))
{

    PrimitiveIterator planes_end = std::stable_partition(m_primitives.begin(), m_primitives.end(), [](const Primitive& v) {
        return std::holds_alternative<Plane>(v);
    });
    size_t count_planes = planes_end - m_primitives.begin();

    m_root = build(planes_end, m_primitives.end());
    std::cout << "count nodes: " << m_nodes.size() << std::endl;
    m_planes_end = m_primitives.begin() + count_planes;
}

ConstPrimitiveIterator BVH::begin() const {
    return m_primitives.begin();
}
ConstPrimitiveIterator BVH::end() const {
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

//number of primitives in <<left>> part and const of partion
using Partion = std::pair<float, size_t>;

static Partion get_optimal_partion(ConstPrimitiveIterator begin, ConstPrimitiveIterator end) {
    size_t count = end - begin;
    std::vector<float> prefix_area(count + 1, 0.f);
    std::vector<float> suffix_area(count + 1, 0.f);

    AABB aabb;

    for (size_t i = 0; i < count; i++) {
        aabb.extend(*(begin + i));
        prefix_area[i + 1] = aabb.area();
    }

    aabb = AABB();
    for (int i = count - 1; i >= 0; i--) {
        aabb.extend(*(begin + i));
        suffix_area[i] = aabb.area();
    }

    auto get_partion = [&prefix_area, &suffix_area, count](size_t i) {
        return std::make_pair(prefix_area[i] * i + suffix_area[i] * (count - i), i);
    };

    Partion result = get_partion(0);
    for (size_t i = 1; i < count; i++) {
        result = std::min(result, get_partion(i));
    }
    return result;
}

size_t BVH::build(PrimitiveIterator begin, PrimitiveIterator end) {
    const static size_t MIN_PARTION_COUNT = 4;

    m_nodes.push_back(Node(-1, -1, begin, end));
    size_t node_idx = m_nodes.size() - 1;

    if (end - begin <= MIN_PARTION_COUNT) {
        return node_idx;
    }

    auto x_copmarator = [](Primitive& first, Primitive& second) {
        return AABB(first).center().x < AABB(second).center().x;
    };
    std::sort(begin, end, x_copmarator);
    Partion best_partion = get_optimal_partion(begin, end);


    auto y_copmarator = [](Primitive& first, Primitive& second) {
        return AABB(first).center().y < AABB(second).center().y;
    };
    std::vector primitives_copy(begin, end);
    std::sort(primitives_copy.begin(), primitives_copy.end(), y_copmarator);

    Partion partion = get_optimal_partion(primitives_copy.begin(), primitives_copy.end());
    if (partion < best_partion) {
        best_partion = partion;
        std::copy(primitives_copy.begin(), primitives_copy.end(), begin);
    }

    auto z_copmarator = [](Primitive &first, Primitive &second) {
        return AABB(first).center().z < AABB(second).center().z;
    };
    std::sort(primitives_copy.begin(), primitives_copy.end(), z_copmarator);

    partion = get_optimal_partion(primitives_copy.begin(), primitives_copy.end());
    if (partion < best_partion) {
        best_partion = partion;
        std::copy(primitives_copy.begin(), primitives_copy.end(), begin);
    }

    if (partion.second == 0) {
        return node_idx;
    }

    m_nodes[node_idx].left_child = build(begin, begin + best_partion.second);
    m_nodes[node_idx].right_child = build(begin + best_partion.second, end);
    return node_idx;
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

    auto add_to_queue = [this, &ray, &queue](size_t node_idx) {
        if (iiintersect(ray, m_nodes[node_idx].aabb).has_value()) {
            queue.push(node_idx);
        }
    };

    queue.push(m_root);
    while (!queue.empty()) {
        const Node& node = m_nodes[queue.front()];
        queue.pop();

        if (node.left_child != -1) {
            add_to_queue(node.left_child);

            assert(node.right_child != -1);
            add_to_queue(node.right_child);
        } else {
            result = closesed(result, intersect(ray, node, max_distance));
        }
    }
    return result;
}

BVH::Node::Node(int left_child, int right_child, ConstPrimitiveIterator begin, ConstPrimitiveIterator end):
    left_child(left_child), right_child(right_child), begin(begin), end(end)
{
    for (auto it = begin; it < end; it++) {
        aabb.extend(*it);
    }
    return;
}
