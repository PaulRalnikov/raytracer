#include "gltf_material.hpp"
#include "glm_parse.hpp"

GltfMaterial::GltfMaterial() :
    base_color_factor(1.f),
    metallic_factor(1),
    emissive_factor(0.f)
{}

GltfMaterial::GltfMaterial(const rapidjson::Value &material) : GltfMaterial() {
    if (material.HasMember("emissiveFactor")) {
        emissive_factor = vec3_from_array(material["emissiveFactor"].GetArray());
    }
    if (material.HasMember("extensions")) {
        const rapidjson::Value &extensions = material["extensions"];
        if (extensions.HasMember("KHR_materials_emissive_strength")) {
            const rapidjson::Value &KHR_materials_emissive_strength = extensions["KHR_materials_emissive_strength"];
            emissive_factor *= KHR_materials_emissive_strength["emissiveStrength"].GetFloat();
        }
    }

    if (!material.HasMember("pbrMetallicRoughness")) {
        return;
    }
    const rapidjson::Value &pbr_metallic_roughtness = material["pbrMetallicRoughness"];
    if (pbr_metallic_roughtness.HasMember("baseColorFactor")) {
        base_color_factor = vec4_from_array(pbr_metallic_roughtness["baseColorFactor"].GetArray());
    }
    if (pbr_metallic_roughtness.HasMember("metallicFactor")) {
        metallic_factor = pbr_metallic_roughtness["metallicFactor"].GetFloat();
    }
}
