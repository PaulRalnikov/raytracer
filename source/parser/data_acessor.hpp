#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <rapidjson/document.h>

#include "types.hpp"

// data, type and component type
using AcessorData = std::tuple<std::vector<char>, std::string, int>;

AcessorData read_acessor_data(
    ConstJsonArray buffer_views,
    const std::vector<std::vector<char>> &buffers_contents,
    const rapidjson::Value &acessor
);
