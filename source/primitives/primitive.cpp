#include "primitive.hpp"

std::ostream& operator<<(std::ostream &out, const Primitive &primitive) {
    auto caller = [&out](const auto& prim) {out << prim;};
    std::visit(caller, primitive);

    return out;
}

MaterialType get_material_type(const Primitive &primitive) {
    auto caller = [](const auto &prim) {return prim.material;};
    return std::visit(caller, primitive);
}

float get_ior(const Primitive &primitive) {
    auto caller = [](const auto &prim) {return prim.ior;};
    return std::visit(caller, primitive);
}

glm::vec3 get_color(const Primitive &primitive) {
    auto caller = [](const auto &prim) {return prim.color;};
    return std::visit(caller, primitive);
}

glm::vec3 get_emission(const Primitive &primitive) {
    auto caller = [](const auto &prim) {return prim.emission;};
    return std::visit(caller, primitive);
}

glm::vec3 get_normal(const Primitive &primitive, glm::vec3 point) {
    struct Visitor{
        glm::vec3 operator()(const Box &prim) { return prim.get_normal(point); }
        glm::vec3 operator()(const Plane& plane) {return plane.get_normal(); }
        glm::vec3 operator()(const Ellipsoid& ellipsoid) { return ellipsoid.get_normal(point); }
        glm::vec3 operator()(const Triangle &triangle) { return triangle.get_normal(); }

        glm::vec3 point;
    };
    return std::visit(Visitor{point}, primitive);
}

std::optional<float> iintersect(const Ray &ray, const Primitive &primitive) {
    auto caller = [&ray](const auto& prim) {
        return intersect(ray, prim);
    };
    return std::visit(caller, primitive);
}
