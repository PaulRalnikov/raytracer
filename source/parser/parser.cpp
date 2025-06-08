#include "parser.hpp"

#include <filesystem>
#include <nlohmann/json.hpp>

#include "glm_parse.hpp"
#include "utils/my_glm.hpp"
#include "gltf_material.hpp"
#include "utils.hpp"
#include "data_acessor.hpp"

static Camera readCamera(const NodeList &node_list, ConstJsonArray cameras, float aspect_ratio) {
    for (size_t i = 0; i < node_list.size(); i++) {
        auto [node, matrix] = node_list[i];
        if (!node.contains("camera")){
            continue;
        }

        int camera_index = node["camera"];
        const auto &camera = cameras[camera_index].get();
        if (!camera.contains("perspective"))
        {
            throw std::runtime_error("Found not perspective camera");
        }
        const auto &camera_params = camera["perspective"];
        float fov_y = camera_params["yfov"];

        return Camera(
            (matrix * glm::vec4(glm::vec3(0.f), 1.f)).xyz(),
            (matrix * glm::vec4(1.0, 0.0, 0.0, 0.0)).xyz(),
            (matrix * glm::vec4(0.0, 1.0, 0.0, 0.0)).xyz(),
            (matrix * glm::vec4(0.0, 0.0, -1.0, 0.0)).xyz(),
            atan(aspect_ratio * tan(fov_y / 2)) * 2,
            fov_y
        );
    }
    throw std::runtime_error("Can not find camera");
}

Parser::Parser(std::filesystem::path path){
    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("File '" + path.string() + "' does not exists");
    }
    in >> json;

    node_list = NodeList(json["nodes"]);
    materials = readArray(json, "materials");
    meshes = readArray(json, "meshes");
    acessors = readArray(json, "accessors");
    buffer_views = readArray(json, "bufferViews");
    buffers = readArray(json, "buffers");
    cameras = readArray(json, "cameras");

    buffers_contents = readBuffersContents(
        buffers,
        std::filesystem::path(path).parent_path()
    );
}

Scene Parser::parseScene(size_t width, size_t height, size_t samples)
{
    const static int DEFAULT_RAY_DEPTH = 6;

    SceneSettings settings(width, height, glm::vec3(0.f), samples, DEFAULT_RAY_DEPTH);

    Camera camera = readCamera(node_list, cameras, (float)width / height);

    std::vector<Triangle> primitives;
    for (size_t node_index = 0; node_index < node_list.size(); node_index++)
    {
        auto [node, translation] = node_list[node_index];
        if (!node.contains("mesh")) {
            continue;
        }

        int mesh_index = node["mesh"];
        const auto &mesh = meshes[mesh_index].get();
        ConstJsonArray mesh_primitives = readArray(mesh, "primitives");

        for (size_t prim_index = 0; prim_index < mesh_primitives.size(); prim_index++)
        {
            const auto &primitive = mesh_primitives[prim_index].get();
            int indexes_acessor_index = primitive["indices"];

            const auto &attributes = primitive["attributes"];

            const auto&indexes_acessor = acessors[indexes_acessor_index].get();
            std::vector<unsigned int> indexes = get_indexes(
                read_acessor_data(buffer_views, buffers_contents, indexes_acessor));

            int position_acessor_index = attributes["POSITION"];
            const auto &position_acessor = acessors[position_acessor_index].get();

            std::vector<glm::vec3> points = get_points(
                read_acessor_data(buffer_views, buffers_contents, position_acessor));


            int normal_acessor_index = attributes["NORMAL"];
            const auto &normal_acessor = acessors[normal_acessor_index].get();

            std::vector<glm::vec3> normals = get_normals(
                read_acessor_data(buffer_views, buffers_contents, normal_acessor));

            if (indexes.size() % 3 != 0) {
                throw std::runtime_error("Error: can not divide indexes into triangles; count: " + std::to_string(indexes.size()));
            }

            GltfMaterial new_material;
            if (primitive.contains("material")) {
                int material_index = primitive["material"];
                new_material = GltfMaterial(materials[material_index]);
            }

            for (size_t index = 0; index < indexes.size(); index += 3) {
                Triangle triangle;
                for (size_t point_index = 0; point_index < 3; point_index++) {
                    triangle.coords[point_index] = translate_point(points[indexes[index + point_index]], translation);
                    triangle.normals[point_index] = glm::normalize(
                        translate_vector(normals[indexes[index + point_index]], translation)
                    );
                }

                triangle.material = MaterialType::DIFFUSE;

                triangle.color = new_material.base_color_factor.xyz();
                if (new_material.base_color_factor.w < 1.0) {
                    triangle.material = MaterialType::DIELECTRIC;
                    triangle.ior = 1.5;
                }
                else if (new_material.metallic_factor > 0) {
                    triangle.material = MaterialType::METALLIC;
                }
                triangle.emission = new_material.emissive_factor;

                primitives.push_back(triangle);
            }
        }
    }

    return Scene(settings, std::move(primitives), camera);
}
