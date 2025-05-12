#include <algorithm>
#include <cmath>
#include <functional>
#include <cmath>
#include <filesystem>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include "distribution/cos_weighted.hpp"
#include "distribution/multi_triangle.hpp"
#include "primitives/triangle.hpp"
#include "scene.hpp"
#include "ray.hpp"
#include "utils/random.hpp"
#include "utils/task_pool.hpp"

static ConstJsonArray readArray(const rapidjson::Document& document, const char* name) {
    const rapidjson::Value& value = document[name];
    return value.GetArray();
}

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
    } else if (data_size_bytes == 3) {
        unsigned int *begin = (unsigned int *)data.data();
        return std::vector<unsigned int>(begin, begin + data.size() / data_size_bytes);
    }
    throw std::runtime_error("Unexpected data_size_bytes: want size_t from 1 to 3, got " + std::to_string(data_size_bytes));
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

std::vector<unsigned int> get_indexes(AcessorData indexes_data) {
    auto [data, type, component_type] = indexes_data;
    if (type != "SCALAR") {
        throw std::runtime_error("Not scalar type in indexes acessor: " + type);
    }
    return get_real_data(data, get_size_in_bytes(component_type));
}

std::vector<glm::vec3> get_points(AcessorData points_data) {
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

static inline glm::vec3 translate(glm::vec3 point, const glm::mat4x4& translate) {
    return (translate * glm::vec4(point, 1.f)).xyz();
}

struct NewMaterial{
    glm::vec4 base_color_factor;
    int metallic_factor;
    glm::vec3 emissive_factor;

    NewMaterial():
        base_color_factor(1.f),
        metallic_factor(1),
        emissive_factor(0.f)
    {}

    NewMaterial(const rapidjson::Value& material): NewMaterial() {
        if (material.HasMember("emissiveFactor")) {
            emissive_factor = vec3_from_array(material["emissiveFactor"].GetArray());
        }
        if (material.HasMember("extensions")) {
            const rapidjson::Value &extensions = material["extensions"];
            if (extensions.HasMember("KHR_materials_emissive_strength")) {
                const rapidjson::Value &KHR_materials_emissive_strength = extensions["KHR_materials_emissive_strength"];
                emissive_factor *= KHR_materials_emissive_strength["emissiveStrength"].GetInt();
            }
        }

        if (!material.HasMember("pbrMetallicRoughness")) {
            return;
        }
        const rapidjson::Value& pbr_metallic_roughtness = material["pbrMetallicRoughness"];
        if (pbr_metallic_roughtness.HasMember("baseColorFactor")) {
            base_color_factor = vec4_from_array(pbr_metallic_roughtness["baseColorFactor"].GetArray());
        }
        if (pbr_metallic_roughtness.HasMember("metallicFactor")) {
            metallic_factor = pbr_metallic_roughtness["metallicFactor"].GetInt();
        }

    }
};

Scene Scene::fromGltf(std::string path, int width, int height, int samples) {
    const static int DEFAULT_RAY_DEPTH = 6;

    std::ifstream in(path);
    if (!in.is_open()) {
        throw std::runtime_error("File '" + path + "' does not exists");
    }
    rapidjson::IStreamWrapper isw(in);

    rapidjson::Document document;

    if (document.ParseStream(isw).HasParseError())
    {
        throw std::runtime_error(
            "rapidjson error: '" +
            std::string(rapidjson::GetParseError_En(document.GetParseError())) +
            "'"
        );
    }

    Scene scene;

    scene.m_width = width;
    scene.m_height = height;
    scene.m_samples = samples;
    scene.m_max_ray_depth = DEFAULT_RAY_DEPTH;
    scene.m_background_color = glm::vec3(0.f);

    NodeList node_list(readArray(document, "nodes"));
    ConstJsonArray cameras = readArray(document, "cameras");

    scene.m_camera = Camera::fromGltfNodes(node_list, cameras, (float) width / height);

    ConstJsonArray meshes = readArray(document, "meshes");
    ConstJsonArray acessors = readArray(document, "accessors");
    ConstJsonArray buffer_views = readArray(document, "bufferViews");
    ConstJsonArray buffers = readArray(document, "buffers");
    ConstJsonArray materials = readArray(document, "materials");

    std::vector<std::vector<char> > buffers_contents = readBuffersContents(
        buffers, std::filesystem::path(path).parent_path()
    );

    std::vector<Triangle> primitives;
    for (size_t node_index = 0; node_index < node_list.size(); node_index++) {
        auto [node, matrix] = node_list[node_index];
        if (!node.HasMember("mesh")) {
            continue;
        }

        int mesh_index = node["mesh"].GetInt();
        const rapidjson::Value& mesh = meshes[mesh_index];
        ConstJsonArray mesh_primitives = mesh["primitives"].GetArray();

        std::cout << "node " << node["name"].GetString() << " size " << mesh_primitives.Size() << std::endl;

        for (rapidjson::SizeType prim_index = 0; prim_index < mesh_primitives.Size(); prim_index++) {
            const rapidjson::Value& primitive = mesh_primitives[prim_index];
            int indexes_acessor_index = primitive["indices"].GetInt();

            const rapidjson::Value &attributes = primitive["attributes"];

            const rapidjson::Value& indexes_acessor = acessors[indexes_acessor_index];
            std::vector<unsigned int> indexes = get_indexes(
                read_acessor_data(buffer_views, buffers_contents, indexes_acessor)
            );

            int position_acessor_index = attributes["POSITION"].GetInt();
            const rapidjson::Value& position_acessor = acessors[position_acessor_index];

            std::vector<glm::vec3> points = get_points(
                read_acessor_data(buffer_views, buffers_contents, position_acessor)
            );
            if (indexes.size() % 3 != 0) {
                throw std::runtime_error("Error: can not divide indexes into triangles; count: " + std::to_string(indexes.size()));
            }

            NewMaterial new_material;
            if (primitive.HasMember("material")) {
                int material_index = primitive["material"].GetInt();
                new_material = NewMaterial(materials[material_index]);
            }

            std::cout << "mesh " << mesh["name"].GetString() << " material: " << std::endl;
            std::cout << "color factor: " << new_material.base_color_factor << std::endl;
            std::cout << "emissive factor: " << new_material.emissive_factor << std::endl;
            std::cout << "metallic factor: " << new_material.metallic_factor << std::endl;
            std::cout << "indexes size: " << indexes.size() << std::endl;
            std::cout << std::endl;

            for (size_t index = 0; index < indexes.size(); index += 3) {
                Triangle triangle;
                for (size_t point_index = 0; point_index < 3; point_index++) {
                    triangle.coords[point_index] = translate(points[indexes[index + point_index]], matrix);
                }

                triangle.material = MaterialType::DIFFUSE;

                triangle.color = new_material.base_color_factor.xyz();
                if (new_material.base_color_factor.w < 1.0) {
                    triangle.material = MaterialType::DIELECTRIC;
                    triangle.ior = 1.5;
                } else if (new_material.metallic_factor > 0) {
                    triangle.material = MaterialType::METALLIC;
                }
                triangle.emission = new_material.emissive_factor;
                if (index == 0) {
                    std::cout << "triangle" << std::endl;
                    std::cout << "material " << to_string(triangle.material) << std::endl;
                    std::cout << "color " << triangle.color << std::endl;
                    std::cout << "emission " << triangle.emission << std::endl;
                    std::cout << std::endl;
                }

                primitives.push_back(triangle);
            }
        }
    }
    std::cout << "primitives count: " << primitives.size() << std::endl;
    scene.m_bvh = BVH(std::move(primitives));
    scene.setup_distribution();

    return scene;
}

std::vector<std::vector<glm::vec3> > Scene::get_pixels() {
    size_t tasks_count = m_width * m_height;
    std::vector<Task> tasks;
    tasks.reserve(tasks_count);

    std::vector<std::vector<std::future<glm::vec3> > > futures(m_height);

    for (int y = 0; y < m_height; y++) {
        futures[y].reserve(m_width);
        for (int x = 0; x < m_width; x++) {
            tasks.push_back(Task(x, y));
            futures[y].push_back(tasks.back().result.get_future());
        }
    }

    auto pool = TaskPool(std::move(tasks), *this);

    std::vector<std::vector<glm::vec3>> pixels(m_height, std::vector<glm::vec3>(m_width));
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            pixels[y][x] = futures[y][x].get();
        }
    }
    return pixels;
}

glm::vec3 Scene::get_pixel_color(int x, int y, pcg32_random_t& rng) const
{
    glm::vec3 color(0.0);
    for (size_t i = 0; i < m_samples; i++)
    {
        glm::vec2 coords = glm::vec2(x, y) + random_vec2(glm::vec3(0), glm::vec2(1), rng);
        Ray ray = ray_to_pixel(coords);
        color += raytrace(ray, rng);
    }
    color /= m_samples;
    return color;
}

glm::vec3 Scene::raytrace(Ray ray, pcg32_random_t &rng, int depth) const
{
    if (depth >= m_max_ray_depth)
    {
        return glm::vec3(0);
    }
    auto intersection = m_bvh.iintersect(ray);
    if (!intersection.has_value())
        return m_background_color;

    const Triangle &triangle = intersection.value().second;

    float t = intersection.value().first; // ray position
    glm::vec3 point = ray.position + ray.direction * t;

    glm::vec3 normal = triangle.get_normal();
    static const float SHIFT = 1e-4;

    float normal_direction_cos = glm::dot(normal, ray.direction);
    bool inside = (normal_direction_cos > 0);
    if (inside)
    {
        normal *= -1;
        normal_direction_cos *= -1;
    }
    glm::vec3 reflected_direction = glm::normalize(ray.direction - 2 * normal_direction_cos * normal);
    reflected_direction = glm::normalize(reflected_direction);
    Ray reflected_ray(point + reflected_direction * SHIFT, reflected_direction);

    switch (triangle.material)
    {
    case (MaterialType::DIELECTRIC):
    {
        // Snell's law
        float mu_1 = 1;
        float mu_2 = triangle.ior;

        float cos_theta_1 = -normal_direction_cos;
        if (inside)
        {
            std::swap(mu_1, mu_2);
        }

        float sin_theta_2 = mu_1 / mu_2 * std::sqrt(1 - std::pow(cos_theta_1, 2));
        if (std::abs(sin_theta_2) > 1)
        {
            return triangle.emission + raytrace(reflected_ray, rng, depth + 1);
        }
        float cos_theta_2 = std::sqrt(1 - std::pow(sin_theta_2, 2));

        // Shlick's approx
        float r_0 = std::pow((mu_1 - mu_2) / (mu_1 + mu_2), 2);
        float r = r_0 + (1 - r_0) * std::pow(1 - cos_theta_1, 5);

        if (random_float(0, 1, rng) < r)
        {
            // choose reflection
            return triangle.emission + raytrace(reflected_ray, rng, depth + 1);
        }
        // choose refraction
        glm::vec3 refracted_direction = mu_1 / mu_2 * ray.direction + (cos_theta_1 * mu_1 / mu_2 - cos_theta_2) * normal;
        refracted_direction = glm::normalize(refracted_direction);
        Ray refracted_ray(point + refracted_direction * SHIFT, refracted_direction);

        glm::vec3 refracted_color = raytrace(refracted_ray, rng, depth + 1);
        if (!inside)
        {
            refracted_color *= triangle.color;
        }
        return triangle.emission + refracted_color;
    }
    case (MaterialType::DIFFUSE):
    {
        glm::vec3 w = m_mis_distribution.sample(point, normal, rng);
        float normal_w_cos = glm::dot(w, normal);
        if (normal_w_cos <= 0 || triangle.color == glm::vec3(0.0))
        {
            return triangle.emission;
        }
        float pdf = m_mis_distribution.pdf(point, normal, w);
        if (pdf == 0.f)
        {
            return triangle.emission;
        }
        Ray random_ray = Ray(point + w * SHIFT, w);
        glm::vec3 L_in = raytrace(random_ray, rng, depth + 1);
        glm::vec3 result = triangle.emission + triangle.color / glm::pi<float>() * L_in * normal_w_cos / pdf;
        return result;
    }
    case (MaterialType::METALLIC):
        glm::vec3 result = triangle.emission + raytrace(reflected_ray, rng, depth + 1) * triangle.color;
        return result;
    }
    throw std::runtime_error("Unsupported type of material");
}

Ray Scene::ray_to_pixel(glm::vec2 pixel) const {
    float tan_fov_x_2 = tan(m_camera.fov_x / 2);
    float tan_fov_y_2 = tan_fov_x_2 * m_height / (float)m_width;

    glm::vec3 position(
        (2 * pixel.x / m_width - 1) * tan_fov_x_2,
        (-2 * pixel.y / m_height + 1) * tan_fov_y_2,
        1
    );

    glm::vec3 direction = glm::normalize(
        position.x * m_camera.right +
        position.y * m_camera.up +
        position.z * m_camera.forward
    );
    return Ray(m_camera.position, direction);
}

void Scene::setup_distribution() {
    // direct light sampling

    std::vector<Triangle> light_triangles;
    for (const auto &el : m_bvh) {
        if (el.emission != glm::vec3(0.0)) {
            light_triangles.emplace_back(el);
        }
    }

    m_mis_distribution.add_distribution(std::make_shared<CosWeighttedDistrubution>());
    if (!light_triangles.empty()) {
        std::cout << "Added dis distribution" << std::endl;
        m_mis_distribution.add_distribution(
            std::make_shared<MultiTriangleDistribution>(std::move(light_triangles))
        );
    }
}
