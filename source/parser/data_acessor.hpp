#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "types.hpp"

// data, type and component type
using DataAcessor = std::tuple<std::vector<char>, std::string, int>;

DataAcessor read_acessor_data(
    ConstJsonArray buffer_views,
    const std::vector<std::vector<char>> &buffers_contents,
    const nlohmann::json &acessor);

std::vector<unsigned int> get_indexes(const DataAcessor& indexes_data);
std::vector<glm::vec3> get_points(DataAcessor points_data);
std::vector<glm::vec3> get_normals(DataAcessor normals_data);
