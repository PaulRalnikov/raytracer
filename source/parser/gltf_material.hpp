#pragma once
#include <glm/glm.hpp>
#include <rapidjson/document.h>

struct GltfMaterial {
    glm::vec4 base_color_factor;
    float metallic_factor;
    glm::vec3 emissive_factor;

    GltfMaterial();
    GltfMaterial(const rapidjson::Value &material);
};
