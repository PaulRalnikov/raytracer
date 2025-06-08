#pragma once
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

struct GltfMaterial {
    glm::vec4 base_color_factor;
    float metallic_factor;
    glm::vec3 emissive_factor;

    GltfMaterial();
    GltfMaterial(const nlohmann::json &material);
};
