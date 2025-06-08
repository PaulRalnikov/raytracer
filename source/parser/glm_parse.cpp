#include "glm_parse.hpp"
#include "types.hpp"

my_quat my_quat_from_array(nlohmann::json array) {
    my_quat q;
    if (array.size() != 4) {
        throw std::runtime_error("Error: array length must be equal to 4");
    }
    q.vec = glm::vec3(
        array[0].get<float>(),
        array[1].get<float>(),
        array[2].get<float>());
    q.w = array[3].get<float>();
    return q;
}

glm::vec3 vec3_from_array(nlohmann::json array) {
    if (array.size() != 3) {
        throw std::runtime_error("Error: array length must be equal to 3");
    }
    return glm::vec3(
        array[0].get<float>(),
        array[1].get<float>(),
        array[2].get<float>());
}

glm::vec4 vec4_from_array(nlohmann::json array) {
    if (array.size() != 4) {
        throw std::runtime_error("Error: array length must be equal to 3");
    }
    return glm::vec4(
        array[0].get<float>(),
        array[1].get<float>(),
        array[2].get<float>(),
        array[3].get<float>());
}

glm::mat4x4 mat4x4_from_array(nlohmann::json array) {
    if (array.size() != 16) {
        throw std::runtime_error("Error: array length must be equal to 16");
    }
    glm::mat4x4 result;
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j < 4; j++) {
            result[i][j] = array[i * 4 + j].get<float>();
        }
    };
    return result;
}
