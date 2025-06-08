#pragma once
#include "utils/my_glm.hpp"
#include "types.hpp"

#include <nlohmann/json.hpp>

glm::vec3 vec3_from_array(nlohmann::json array);
glm::vec4 vec4_from_array(nlohmann::json array);
glm::mat4x4 mat4x4_from_array(nlohmann::json array);
my_quat my_quat_from_array(nlohmann::json array);

