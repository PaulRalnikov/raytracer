#pragma once
#include <variant>
#include <iostream>

#include "box.hpp"
#include "plane.hpp"
#include "ellipsoid.hpp"

using Primitive = std::variant<Box, Plane, Ellipsoid>;

std::ostream& operator<<(std::ostream& out, const Primitive& primitive);

MaterialType get_material_type(const Primitive& primitive);
float get_ior(const Primitive& primitive);
glm::vec3 get_color(const Primitive& primitive);
glm::vec3 get_emission(const Primitive& primitive);

glm::vec3 get_normal(const Primitive &primitive, glm::vec3 point);
glm::vec3 get_unconverted_normal(const Primitive &primitive, glm::vec3 point);
std::optional<float> iintersect(const Ray &ray, const Primitive &primitive);
