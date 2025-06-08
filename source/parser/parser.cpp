#include "parser.hpp"

#include <filesystem>

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include "glm_parse.hpp"

static ConstJsonArray readArray(const rapidjson::Document& document, const char* name) {
    const rapidjson::Value& value = document[name];
    return value.GetArray();
}


static inline glm::vec3 point_translate(glm::vec3 point, const glm::mat4x4 &translate) {
    return (translate * glm::vec4(point, 1.f)).xyz();
}

static inline glm::vec3 vec_translate(glm::vec3 vec, const glm::mat4x4 &translate) {
    return (translate * glm::vec4(vec, 0.f)).xyz();
}

struct NewMaterial {
    glm::vec4 base_color_factor;
    float metallic_factor;
    glm::vec3 emissive_factor;

    NewMaterial() : base_color_factor(1.f),
                    metallic_factor(1),
                    emissive_factor(0.f)
    {}

    NewMaterial(const rapidjson::Value &material) : NewMaterial(){
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
};


static std::vector<std::vector<char> > readBuffersContents(
    ConstJsonArray buffers,
    std::filesystem::path buffers_dir_path)
{
    std::vector<std::vector<char> > result(buffers.Size());
    for (rapidjson::SizeType i = 0; i < buffers.Size(); i++) {
        size_t byte_length = buffers[i]["byteLength"].GetUint();
        result[i].resize(byte_length);

        std::string uri_name = buffers[i]["uri"].GetString();
        std::ifstream in(buffers_dir_path / uri_name, std::ios::binary);
        in.read(result[i].data(), byte_length);
    }
    return result;
}

static int get_size_in_bytes(int componentType) {
    if (componentType == 5120 || componentType == 5121) {
        return 1;
    }
    if (componentType == 5122 || componentType == 5123) {
        return 2;
    }
    if (componentType == 5125 || componentType == 5126) {
        return 4;
    }
    throw std::runtime_error("Unexpected data type: " + std::to_string(componentType));
}

static std::vector<unsigned int> get_real_data(const std::vector<char>& data, size_t data_size_bytes) {
    if (data_size_bytes == 1) {
        return std::vector<unsigned int>(data.data(), data.data() + data.size());
    } else if (data_size_bytes == 2) {
        unsigned short* begin = (unsigned short*)data.data();
        return std::vector<unsigned int>(begin, begin + data.size() / data_size_bytes);
    } else if (data_size_bytes == 4) {
        unsigned int *begin = (unsigned int *)data.data();
        return std::vector<unsigned int>(begin, begin + data.size() / data_size_bytes);
    }
    throw std::runtime_error("Unexpected data_size_bytes: want size_t from 1, 2 or 4, got " + std::to_string(data_size_bytes));
}

static size_t get_type_number_of_compoents(std::string type) {
    if (type == "SCALAR") {
        return 1;
    } else if (type == "VEC3") {
        return 3;
    }
    throw std::runtime_error("Unexpected type: " + type);
}

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
    Camera result;
    for (rapidjson::SizeType i = 0; i < node_list.size(); i++) {
        auto [node, matrix] = node_list[i];
        if (!node.HasMember("camera")){
            continue;
        }

        result.right = (matrix * glm::vec4(1.0, 0.0, 0.0, 0.0)).xyz();
        result.up = (matrix * glm::vec4(0.0, 1.0, 0.0, 0.0)).xyz();
        result.forward = (matrix * glm::vec4(0.0, 0.0, -1.0, 0.0)).xyz();

        result.position = (matrix * glm::vec4(glm::vec3(0.f), 1.f)).xyz();

        int camera_index = node["camera"].GetInt();
        const auto &camera = cameras[camera_index];
        if (!camera.HasMember("perspective")) {
            throw std::runtime_error("Found not perspective camera");
        }

        const auto &camera_params = camera["perspective"];
        result.fov_y = camera_params["yfov"].GetFloat();
        result.fov_x = atan(aspect_ratio * tan(result.fov_y / 2)) * 2;

        break;
    }
    return result;
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

    Scene scene;

    scene.m_width = width;
    scene.m_height = height;
    scene.m_samples = samples;
    scene.m_max_ray_depth = DEFAULT_RAY_DEPTH;
    scene.m_background_color = glm::vec3(0.f);

    NodeList node_list(readArray(document, "nodes"));
    ConstJsonArray materials = readArray(document, "materials");
    ConstJsonArray meshes = readArray(document, "meshes");
    ConstJsonArray acessors = readArray(document, "accessors");
    ConstJsonArray buffer_views = readArray(document, "bufferViews");
    ConstJsonArray buffers = readArray(document, "buffers");

    ConstJsonArray cameras = readArray(document, "cameras");
    scene.m_camera = readCamera(node_list, cameras, (float)width / height);

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

            NewMaterial new_material;
            if (primitive.HasMember("material")) {
                int material_index = primitive["material"].GetInt();
                new_material = NewMaterial(materials[material_index]);
            }

            for (size_t index = 0; index < indexes.size(); index += 3) {
                Triangle triangle;
                for (size_t point_index = 0; point_index < 3; point_index++) {
                    triangle.coords[point_index] = point_translate(points[indexes[index + point_index]], translation);
                    triangle.normals[point_index] = point_translate(normals[indexes[index + point_index]], translation);
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
    scene.m_bvh = BVH(std::move(primitives));
    scene.setup_distribution();

    return scene;
}
