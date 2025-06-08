#include "gltf_material.hpp"
#include "glm_parse.hpp"

GltfMaterial::GltfMaterial() :
    base_color_factor(1.f),
    metallic_factor(1),
    emissive_factor(0.f)
{}

GltfMaterial::GltfMaterial(const nlohmann::json &material) : GltfMaterial() {
    if (material.contains("emissiveFactor")) {
        emissive_factor = vec3_from_array(material["emissiveFactor"]);
    }
    if (material.contains("extensions")) {
        const auto &extensions = material["extensions"];
        if (extensions.contains("KHR_materials_emissive_strength")) {
            const auto &KHR_materials_emissive_strength = extensions["KHR_materials_emissive_strength"];
            emissive_factor *= KHR_materials_emissive_strength["emissiveStrength"];
        }
    }

    if (!material.contains("pbrMetallicRoughness")) {
        return;
    }
    const auto &pbr_metallic_roughtness = material["pbrMetallicRoughness"];
    if (pbr_metallic_roughtness.contains("baseColorFactor")) {
        base_color_factor = vec4_from_array(pbr_metallic_roughtness["baseColorFactor"]);
    }
    if (pbr_metallic_roughtness.contains("metallicFactor")) {
        metallic_factor = pbr_metallic_roughtness["metallicFactor"];
    }
}
