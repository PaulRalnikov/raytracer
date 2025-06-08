#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <rapidjson/document.h>

#include "types.hpp"

// data, type and component type
using DataAcessor = std::tuple<std::vector<char>, std::string, int>;

DataAcessor read_acessor_data(
    ConstJsonArray buffer_views,
    const std::vector<std::vector<char>> &buffers_contents,
    const rapidjson::Value &acessor
);
