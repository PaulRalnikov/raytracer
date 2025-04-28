#include "primitive.hpp"
#include "box.hpp"
#include "ellipsoid.hpp"
#include "triangle.hpp"

std::optional<float> intersect(const Ray &ray, const FinitePrimitive &primitive) {
    return std::visit([&ray](const auto& prim) {
        return intersect(ray, prim);
    }, primitive);
}

PrimitiveDistribution::PrimitiveDistribution(std::vector<FinitePrimitive>&& primitives):
    m_primitives(std::move(primitives)) {}

glm::vec3 PrimitiveDistribution::sample(glm::vec3 point, glm::vec3 normal, pcg32_random_t &rng) const {
    size_t i = random_int(0, m_primitives.size() - 1, rng);
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

        const glm::vec3& point;
        const glm::vec3& normal;
        pcg32_random_t& rng;
    };
    return std::visit(Visitor{point, normal, rng}, m_primitives[i]);
}

float PrimitiveDistribution::pdf(glm::vec3 point, glm::vec3 normal, glm::vec3 direction) const {
    float sum = 0;
    Ray ray(point, direction);
    for (const auto& primitive : m_primitives) {
        std::optional<float> intersection = intersect(ray, primitive);
        if (!intersection.has_value()) {
            continue;
        }
        float ray_length = intersection.value();
        struct Visitor{
            float operator()(const Box& box) {
                return ppdf(box, ray, ray_length);
            };
            float operator()(const Ellipsoid& ellipsoid) {
                return ppdf(ellipsoid, point, normal, direction);
            };
            float operator()(const Triangle& triangle) {
                return ppdf(triangle, ray, ray_length);
            };

            const glm::vec3& point;
            const glm::vec3& normal;
            const glm::vec3& direction;
            const Ray& ray;
            float ray_length;
        };
        sum += std::visit(Visitor{point, normal, direction, ray, ray_length}, primitive);
    }
    return sum / m_primitives.size();
}
