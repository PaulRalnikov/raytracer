#pragma once
#include <vector>
#include "primitives/primitive.hpp"
#include "utils/random.hpp"

using Intersection = std::optional<std::pair<float, std::reference_wrapper<const Primitive> > >;

class BVH {
public:
    BVH(std::vector<Primitive>&& primitives = {});

    std::vector<Primitive>::const_iterator begin() const;
    std::vector<Primitive>::const_iterator end() const;

    Intersection intersect(
        Ray ray,
        float max_distance = std::numeric_limits<float>::infinity()
    ) const;
private:
    std::vector<Primitive> m_primitives;
};
