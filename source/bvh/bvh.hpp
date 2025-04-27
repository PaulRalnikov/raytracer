#pragma once
#include <vector>
#include "primitives/primitive.hpp"
#include "utils/random.hpp"
#include "aabb.hpp"

using Intersection = std::optional<std::pair<float, std::reference_wrapper<const Primitive>>>;
using ConstPrimitiveIterator = std::vector<Primitive>::const_iterator;
using PrimitiveIterator = std::vector<Primitive>::iterator;
class BVH {
public:
    BVH(std::vector<Primitive>&& primitives = {});

    ConstPrimitiveIterator begin() const;
    ConstPrimitiveIterator end() const;

    Intersection intersect(
        Ray ray,
        float max_distance = std::numeric_limits<float>::infinity()
    ) const;
private:
    //return index of node in m_nodes
    size_t build(PrimitiveIterator begin, PrimitiveIterator end);

    struct Node{
        int left_child;
        int right_child;
        ConstPrimitiveIterator begin;
        ConstPrimitiveIterator end;
        AABB aabb;

        Node(int left_child, int right_child, ConstPrimitiveIterator begin, ConstPrimitiveIterator end);
    };

    Intersection intersect(
        const Ray& ray,
        const Node& node,
        float max_distance = std::numeric_limits<float>::infinity()
    ) const;

    Intersection intersect_with_nodes(
        const Ray &ray,
        float max_distance = std::numeric_limits<float>::infinity()
    ) const;

    std::vector<Primitive> m_primitives;
    std::vector<Node> m_nodes;
    ConstPrimitiveIterator m_planes_end;
    size_t m_root;
};
