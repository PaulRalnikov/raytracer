#include "data_acessor.hpp"

#include <vector>

#include "types.hpp"
#include "utils.hpp"

DataAcessor read_acessor_data(
    ConstJsonArray buffer_views,
    const std::vector<std::vector<char> >& buffers_contents,
    const nlohmann::json& acessor
) {
    int buffer_view_index = acessor["bufferView"];
    int component_type = acessor["componentType"];
    int count = acessor["count"];
    int bytes_count = get_size_in_bytes(component_type);

    std::string type = acessor["type"];
    size_t components_count = get_type_number_of_compoents(type);

    int byte_offset = 0;
    if (acessor.contains("byteOffset")) {
        byte_offset = acessor["byteOffset"];
    }

    const auto& buffer_view = buffer_views[buffer_view_index].get();
    int buffer_index = buffer_view["buffer"];

    int buffer_view_offset = buffer_view["byteOffset"];
    int start = byte_offset + buffer_view_offset;

    auto it_start = buffers_contents[buffer_index].begin() + start;
    auto it_end = it_start + count * bytes_count * components_count;

    return {std::vector<char> (it_start, it_end), type, component_type};
}

std::vector<unsigned int> get_indexes(const DataAcessor& indexes_data) {
    auto [data, type, component_type] = indexes_data;
    if (type != "SCALAR") {
        throw std::runtime_error("Not scalar type in indexes acessor: " + type);
    }
    return get_real_data(data, get_size_in_bytes(component_type));
}

std::vector<glm::vec3> get_points(DataAcessor points_data) {
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

std::vector<glm::vec3> get_normals(DataAcessor normals_data) {
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
