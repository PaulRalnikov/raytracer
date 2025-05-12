#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vec_swizzle.hpp>
#include <glm/glm.hpp>


#include "utils/my_glm.hpp"

Camera Camera::fromGltfNodes(const NodeList& node_list, ConstJsonArray cameras, float aspect_ratio) {
    Camera result;
    for (rapidjson::SizeType i = 0; i < node_list.size(); i++)
    {
        auto [node, matrix] = node_list[i];
        if (!node.HasMember("camera")) {
            continue;
        }

        result.right = (matrix * glm::vec4(1.0, 0.0, 0.0, 0.0)).xyz();
        result.up = (matrix * glm::vec4(0.0, 1.0, 0.0, 0.0)).xyz();
        result.forward = (matrix * glm::vec4(0.0, 0.0, -1.0, 0.0)).xyz();

        result.position = (matrix * glm::vec4(glm::vec3(0.f), 1.f)).xyz();

        int camera_index = node["camera"].GetInt();
        const auto &camera = cameras[camera_index];
        if (!camera.HasMember("perspective")){
            throw std::runtime_error("Found not perspective camera");
        }

        const auto &camera_params = camera["perspective"];
        result.fov_y = camera_params["yfov"].GetFloat();
        result.fov_x = atan(aspect_ratio * tan(result.fov_y / 2)) * 2;

        break;
    }
    return result;
}
