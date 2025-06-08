#pragma once
#include <vector>
#include "primitives/triangle.hpp"
#include "utils/random.hpp"
#include "aabb.hpp"

using Intersection = std::optional<std::pair<glm::vec3, std::reference_wrapper<const Triangle>>>;
using ConstTriangleIterator = std::vector<Triangle>::const_iterator;
using TriangleIterator = std::vector<Triangle>::iterator;
class BVH {
public:
    BVH(std::vector<Triangle>&& primitives = {});

    ConstTriangleIterator begin() const;
    ConstTriangleIterator end() const;
    size_t size() const;

    const Triangle& operator[](size_t i) const;

    Intersection iintersect(
        Ray ray,
        float max_distance = std::numeric_limits<float>::infinity()
    ) const;

    // returns pdf of direction distribution over all primitives on bvh
    float pdf(const Ray &ray) const;
private:
    //return index of node in m_nodes
    size_t build(TriangleIterator begin, TriangleIterator end);

    struct Node{
        int left_child;
        int right_child;
        ConstTriangleIterator begin;
        ConstTriangleIterator end;
        AABB aabb;

        Node(int left_child, int right_child, ConstTriangleIterator begin, ConstTriangleIterator end);
    };

    Intersection intersect_with_node(
        const Ray& ray,
        const Node& node,
        float max_distance = std::numeric_limits<float>::infinity()
    ) const;

    std::vector<Triangle> m_triangles;
    std::vector<Node> m_nodes;
    size_t m_root;
};
