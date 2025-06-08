#pragma once
#include <rapidjson/document.h>
#include <vector>
#include <filesystem>

#include "types.hpp"

ConstJsonArray readArray(const rapidjson::Document &document, const char *name);

std::vector<std::vector<char>> readBuffersContents(
    ConstJsonArray buffers,
    std::filesystem::path buffers_dir_path
);

int get_size_in_bytes(int componentType);
size_t get_type_number_of_compoents(std::string type);
std::vector<unsigned int> get_real_data(const std::vector<char> &data, size_t data_size_bytes);
