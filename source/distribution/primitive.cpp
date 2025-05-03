#include "primitive.hpp"
#include "box.hpp"
#include "ellipsoid.hpp"
#include "triangle.hpp"

std::optional<float> intersect(const Ray &ray, const FinitePrimitive &primitive) {
    return std::visit([&ray](const auto& prim) {
        return intersect(ray, prim);
    }, primitive);
}

PrimitiveDistribution::PrimitiveDistribution(std::vector<FinitePrimitive>&& primitives)
{
    auto visit = [](const FinitePrimitive& primitive) {
        return std::visit([](const auto& prim){
            return Primitive(prim);
        }, primitive);
    };

    std::vector<Primitive> fucking_primitives;
    std::transform(
        primitives.begin(),
        primitives.end(),
        std::back_insert_iterator(fucking_primitives),
        visit
    );
    m_bvh = BVH(std::move(fucking_primitives));
}

glm::vec3 PrimitiveDistribution::sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const {
    size_t i = random_int(0, m_bvh.size() - 1, rng);
    struct Visitor{
        glm::vec3 operator()(const Box& box) {
            return ssample(box, point, normal, rng);
        };
        glm::vec3 operator()(const Ellipsoid &ellipsoid) {
            return ssample(ellipsoid, point, normal, rng);
        };
        glm::vec3 operator()(const Triangle &triangle) {
            return ssample(triangle, point, rng);
        };
        glm::vec3 operator()(const Plane& plane) {
            throw std::runtime_error("Plane in primitive distribution!");
        }

        const glm::vec3& point;
        const glm::vec3& normal;
        pcg32_random_t& rng;
    };
    return std::visit(Visitor{point, normal, rng}, m_bvh[i]);
}

float PrimitiveDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    Ray ray(point, direction);
    return m_bvh.pdf(ray);
}
