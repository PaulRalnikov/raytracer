#include "bvh.hpp"
#include <queue>
#include "primitives/triangle.hpp"
#include "distribution/triangle.hpp"

BVH::BVH(std::vector<Triangle>&& primitives):
    m_triangles(std::move(primitives))
{
    m_root = build(m_triangles.begin(), m_triangles.end());
}

ConstTriangleIterator BVH::begin() const {
    return m_triangles.begin();
}
ConstTriangleIterator BVH::end() const {
    return m_triangles.end();
}

size_t BVH::size() const {
    return end() - begin();
}

const Triangle& BVH::operator[](size_t i) const {
    return m_triangles[i];
}

void static inline update_intersection(
    Intersection& intersection,
    std::optional<float> update,
    const Triangle& triangle)
{
    if (update.has_value() && (!intersection.has_value() || update.value() < intersection.value().first)) {
        intersection = std::make_pair(update.value(), std::cref(triangle));
    }
}

static inline Intersection closesed(const Intersection &first, const Intersection &second) {
    return (
        first.has_value() && (!second.has_value() || first.value().first < second.value().first)
        ? first
        : second
    );
}

Intersection BVH::iintersect(Ray ray, float max_distance) const
{
    Intersection result = {};

    std::queue<size_t> queue;

    auto add_to_queue = [this, &ray, &queue, &result](size_t node_idx) {
        auto intersection = intersect(ray, m_nodes[node_idx].aabb);
        if (intersection.has_value() && (!result.has_value() || intersection.value() < result.value().first)) {
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
            result = closesed(result, intersect_with_node(ray, node, max_distance));
        }
    }
    return result;
}

// cost of partion and number of primitives in <<left>> part
using Partion = std::pair<float, size_t>;

static Partion get_optimal_partion(ConstTriangleIterator begin, ConstTriangleIterator end) {
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

size_t BVH::build(TriangleIterator begin, TriangleIterator end) {
    const static size_t MIN_PARTION_COUNT = 3;

    m_nodes.push_back(Node(-1, -1, begin, end));
    size_t node_idx = m_nodes.size() - 1;

    if (end - begin <= MIN_PARTION_COUNT) {
        return node_idx;
    }

    auto x_copmarator = [](Triangle& first, Triangle& second) {
        return AABB(first).center().x < AABB(second).center().x;
    };
    std::sort(begin, end, x_copmarator);
    Partion best_partion = get_optimal_partion(begin, end);


    auto y_copmarator = [](Triangle& first, Triangle& second) {
        return AABB(first).center().y < AABB(second).center().y;
    };
    std::vector triangles_copy(begin, end);
    std::sort(triangles_copy.begin(), triangles_copy.end(), y_copmarator);

    Partion partion = get_optimal_partion(triangles_copy.begin(), triangles_copy.end());
    if (partion < best_partion) {
        best_partion = partion;
        std::copy(triangles_copy.begin(), triangles_copy.end(), begin);
    }

    auto z_copmarator = [](Triangle &first, Triangle &second) {
        return AABB(first).center().z < AABB(second).center().z;
    };
    std::sort(triangles_copy.begin(), triangles_copy.end(), z_copmarator);

    partion = get_optimal_partion(triangles_copy.begin(), triangles_copy.end());
    if (partion < best_partion) {
        best_partion = partion;
        std::copy(triangles_copy.begin(), triangles_copy.end(), begin);
    }

    if (partion.second == 0) {
        return node_idx;
    }

    m_nodes[node_idx].left_child = build(begin, begin + best_partion.second);
    m_nodes[node_idx].right_child = build(begin + best_partion.second, end);
    return node_idx;
}

Intersection BVH::intersect_with_node(
    const Ray& ray,
    const BVH::Node& node,
    float max_distance) const
{
    Intersection intersection = {};
    for (auto it = node.begin; it < node.end; it++) {
        Triangle t = *it;
        update_intersection(intersection, intersect(ray, t), *it);
    }
    return intersection;
}

static inline float get_primitive_pdf(const Ray& ray, const Triangle& triangle) {
    std::optional<float> intersection = intersect(ray, triangle);

    if (!intersection.has_value()) {
        return 0;
    }

    float ray_length = intersection.value();
    return ppdf(triangle, ray, ray_length);
}

float BVH::pdf(const Ray& ray) const {
    float sum = 0;

    std::queue<size_t> queue;

    auto add_to_queue = [this, &ray, &queue](size_t node_idx) {
        auto intersection = intersect(ray, m_nodes[node_idx].aabb);
        if (intersection.has_value()) {
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
            for (auto it = node.begin; it < node.end; it++) {
                sum += get_primitive_pdf(ray, *it);
            }
        }
    }
    return sum / (m_triangles.end() - m_triangles.begin());
}

BVH::Node::Node(int left_child, int right_child, ConstTriangleIterator begin, ConstTriangleIterator end):
    left_child(left_child), right_child(right_child), begin(begin), end(end)
{
    for (auto it = begin; it < end; it++) {
        aabb.extend(*it);
    }
    return;
}
