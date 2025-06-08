#include "parser.hpp"

#include <filesystem>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include "glm_parse.hpp"
#include "utils/my_glm.hpp"
#include "gltf_material.hpp"
#include "utils.hpp"

//data, type and component type
using AcessorData = std::tuple<std::vector<char>, std::string, int>;

static AcessorData read_acessor_data(
    ConstJsonArray buffer_views,
    const std::vector<std::vector<char> >& buffers_contents,
    const rapidjson::Value& acessor
) {
    int buffer_view_index = acessor["bufferView"].GetInt();
    int component_type = acessor["componentType"].GetInt();
    int count = acessor["count"].GetInt();
    int bytes_count = get_size_in_bytes(component_type);

    std::string type = acessor["type"].GetString();
    size_t components_count = get_type_number_of_compoents(type);

    int byte_offset = 0;
    if (acessor.HasMember("byteOffset")) {
        byte_offset = acessor["byteOffset"].GetInt();
    }

    const rapidjson::Value& buffer_view = buffer_views[buffer_view_index];
    int buffer_index = buffer_view["buffer"].GetInt();

    int start = byte_offset + buffer_view["byteOffset"].GetInt();

    auto it_start = buffers_contents[buffer_index].begin() + start;
    auto it_end = it_start + count * bytes_count * components_count;

    return {std::vector<char> (it_start, it_end), type, component_type};
}

static std::vector<unsigned int> get_indexes(AcessorData indexes_data) {
    auto [data, type, component_type] = indexes_data;
    if (type != "SCALAR") {
        throw std::runtime_error("Not scalar type in indexes acessor: " + type);
    }
    return get_real_data(data, get_size_in_bytes(component_type));
}

static std::vector<glm::vec3> get_points(AcessorData points_data) {
    auto [data, type, component_type] = points_data;
    if (type != "VEC3") {
        throw std::runtime_error("Not VEC3 type in position acessor: " + type);
    }
    if (get_size_in_bytes(component_type) != 4) {
        throw std::runtime_error("Unexpected component type: want 4-bytes type, got " + component_type);
    }
    glm::vec3* begin = (glm::vec3*)data.data();
    return std::vector<glm::vec3>(begin, begin + data.size() / sizeof(glm::vec3));
}

static std::vector<glm::vec3> get_normals(AcessorData normals_data) {
    auto [data, type, component_type] = normals_data;
    if (type != "VEC3") {
        throw std::runtime_error("Not VEC3 type in normal acessor: " + type);
    }
    if (get_size_in_bytes(component_type) != 4) {
        throw std::runtime_error("Unexpected component type: want 4-bytes type, got " + component_type);
    }
    glm::vec3* begin = (glm::vec3*)data.data();
    return std::vector<glm::vec3>(begin, begin + data.size() / sizeof(glm::vec3));
}

static Camera readCamera(const NodeList &node_list, ConstJsonArray cameras, float aspect_ratio) {
    for (rapidjson::SizeType i = 0; i < node_list.size(); i++) {
        auto [node, matrix] = node_list[i];
        if (!node.HasMember("camera")){
            continue;
        }

        int camera_index = node["camera"].GetInt();
        const auto &camera = cameras[camera_index];
        if (!camera.HasMember("perspective"))
        {
            throw std::runtime_error("Found not perspective camera");
        }
        const auto &camera_params = camera["perspective"];
        float fov_y = camera_params["yfov"].GetFloat();

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

Scene Parser::parse(std::string path, int width, int height, int samples)
{
    const static int DEFAULT_RAY_DEPTH = 6;

    std::ifstream in(path);
    if (!in.is_open())
    {
        throw std::runtime_error("File '" + path + "' does not exists");
    }
    rapidjson::IStreamWrapper isw(in);

    rapidjson::Document document;

    if (document.ParseStream(isw).HasParseError())
    {
        throw std::runtime_error(
            "rapidjson error: '" +
            std::string(rapidjson::GetParseError_En(document.GetParseError())) +
            "'");
    }

    SceneSettings settings(width, height, glm::vec3(0.f), samples, DEFAULT_RAY_DEPTH);

    NodeList node_list(readArray(document, "nodes"));
    ConstJsonArray materials = readArray(document, "materials");
    ConstJsonArray meshes = readArray(document, "meshes");
    ConstJsonArray acessors = readArray(document, "accessors");
    ConstJsonArray buffer_views = readArray(document, "bufferViews");
    ConstJsonArray buffers = readArray(document, "buffers");

    ConstJsonArray cameras = readArray(document, "cameras");
    Camera camera = readCamera(node_list, cameras, (float)width / height);

    std::vector<std::vector<char>> buffers_contents = readBuffersContents(
        buffers, std::filesystem::path(path).parent_path());

    std::vector<Triangle> primitives;
    for (size_t node_index = 0; node_index < node_list.size(); node_index++)
    {
        auto [node, translation] = node_list[node_index];
        if (!node.HasMember("mesh")) {
            continue;
        }

        int mesh_index = node["mesh"].GetInt();
        const rapidjson::Value &mesh = meshes[mesh_index];
        ConstJsonArray mesh_primitives = mesh["primitives"].GetArray();

        for (rapidjson::SizeType prim_index = 0; prim_index < mesh_primitives.Size(); prim_index++)
        {
            const rapidjson::Value &primitive = mesh_primitives[prim_index];
            int indexes_acessor_index = primitive["indices"].GetInt();

            const rapidjson::Value &attributes = primitive["attributes"];

            const rapidjson::Value &indexes_acessor = acessors[indexes_acessor_index];
            std::vector<unsigned int> indexes = get_indexes(
                read_acessor_data(buffer_views, buffers_contents, indexes_acessor));

            int position_acessor_index = attributes["POSITION"].GetInt();
            const rapidjson::Value &position_acessor = acessors[position_acessor_index];

            std::vector<glm::vec3> points = get_points(
                read_acessor_data(buffer_views, buffers_contents, position_acessor));


            int normal_acessor_index = attributes["NORMAL"].GetInt();
            const rapidjson::Value &normal_acessor = acessors[normal_acessor_index];

            std::vector<glm::vec3> normals = get_normals(
                read_acessor_data(buffer_views, buffers_contents, normal_acessor));

            if (indexes.size() % 3 != 0) {
                throw std::runtime_error("Error: can not divide indexes into triangles; count: " + std::to_string(indexes.size()));
            }

            GltfMaterial new_material;
            if (primitive.HasMember("material")) {
                int material_index = primitive["material"].GetInt();
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
