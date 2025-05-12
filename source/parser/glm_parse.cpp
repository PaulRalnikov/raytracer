#include "glm_parse.hpp"
#include "types.hpp"

my_quat my_quat_from_array(ConstJsonArray array) {
    my_quat q;
    if (array.Size() != 4) {
        throw std::runtime_error("Error: array length must be equal to 4");
    }
    q.vec = glm::vec3(
        array[0].GetFloat(),
        array[1].GetFloat(),
        array[2].GetFloat()
    );
    q.w = array[3].GetFloat();
    return q;
}

glm::vec3 vec3_from_array(ConstJsonArray array) {
    if (array.Size() != 3) {
        throw std::runtime_error("Error: array length must be equal to 3");
    }
    return glm::vec3(
        array[0].GetFloat(),
        array[1].GetFloat(),
        array[2].GetFloat()
    );
}

glm::vec4 vec4_from_array(ConstJsonArray array) {
    if (array.Size() != 4) {
        throw std::runtime_error("Error: array length must be equal to 3");
    }
    return glm::vec4(
        array[0].GetFloat(),
        array[1].GetFloat(),
        array[2].GetFloat(),
        array[3].GetFloat()
    );
}

glm::mat4x4 mat4x4_from_array(ConstJsonArray array) {
    if (array.Size() != 16) {
        throw std::runtime_error("Error: array length must be equal to 16");
    }
    glm::mat4x4 result;
    for (size_t i = 0; i < 4; i++) {
        for (size_t j = 0; j, 4; j++) {
            result[i][j] = array[i * 4 + j].GetFloat();
        }
    };
    return result;
}
