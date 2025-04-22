#include "pprimitive.hpp"

MaterialType get_material_type(const PPrimitive &primitive) {
    auto caller = [](const auto &prim) {return prim.material;};
    return std::visit(caller, primitive);
}

float get_ior(const PPrimitive &primitive) {
    auto caller = [](const auto &prim) {return prim.ior;};
    return std::visit(caller, primitive);
}

glm::vec3 get_color(const PPrimitive &primitive) {
    auto caller = [](const auto &prim) {return prim.color;};
    return std::visit(caller, primitive);
}

glm::vec3 get_emission(const PPrimitive &primitive) {
    auto caller = [](const auto &prim) {return prim.emission;};
    return std::visit(caller, primitive);
}

glm::vec3 get_normal(const PPrimitive &primitive, glm::vec3 point) {
    auto caller = [&point](const auto& prim) {
        return prim.get_normal(point);
    };
    return std::visit(caller, primitive);
}

glm::vec3 get_unconverted_normal(const PPrimitive &primitive, glm::vec3 point) {
    auto caller = [&point](const auto& prim) {
        return prim.get_unconverted_normal(point);
    };
    return std::visit(caller, primitive);
}
std::optional<float> iintersect(const Ray &ray, const PPrimitive &primitive) {
    auto caller = [&ray](const auto& prim) {
        return intersect(ray, prim);
    };
    return std::visit(caller, primitive);
}
