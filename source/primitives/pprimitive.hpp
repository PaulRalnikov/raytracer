#pragma once
#include <variant>

#include "box.hpp"
#include "primitive.hpp"

using PPrimitive = std::variant<Box, Primitive>;

MaterialType get_material_type(const PPrimitive& primitive);
float get_ior(const PPrimitive& primitive);
glm::vec3 get_color(const PPrimitive& primitive);
glm::vec3 get_emission(const PPrimitive& primitive);

glm::vec3 get_normal(const PPrimitive &primitive, glm::vec3 point);
glm::vec3 get_unconverted_normal(const PPrimitive &primitive, glm::vec3 point);
std::optional<float> iintersect(const Ray &ray, const PPrimitive &primitive);
