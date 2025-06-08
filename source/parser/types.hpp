#pragma once
#include <nlohmann/json.hpp>
#include <functional>

using ConstJsonArray = std::vector<std::reference_wrapper<const nlohmann::json> >;

