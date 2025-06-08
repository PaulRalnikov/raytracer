#include "utils.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

ConstJsonArray readArray(const nlohmann::json& json, const char* name) {
    ConstJsonArray result;
    for (const auto& node : json[name]) {
        result.push_back(std::cref(node));
    }
    return result;
}

std::vector<std::vector<char>> readBuffersContents(
    ConstJsonArray buffers,
    std::filesystem::path buffers_dir_path)
{
    std::vector<std::vector<char>> result(buffers.size());
    for (size_t i = 0; i < buffers.size(); i++)
    {
        size_t byte_length = buffers[i].get()["byteLength"];
        result[i].resize(byte_length);

        std::string uri_name = buffers[i].get()["uri"];
        std::ifstream in(buffers_dir_path / uri_name, std::ios::binary);
        in.read(result[i].data(), byte_length);
    }
    return result;
}

int get_size_in_bytes(int componentType) {
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

size_t get_type_number_of_compoents(std::string type) {
    if (type == "SCALAR") {
        return 1;
    } else if (type == "VEC3") {
        return 3;
    }
    throw std::runtime_error("Unexpected type: " + type);
}

std::vector<unsigned int> get_real_data(const std::vector<char>& data, size_t data_size_bytes) {
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
