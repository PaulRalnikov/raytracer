#include "camera.hpp"
#include "utils/my_glm.hpp"

Camera Camera::fromGltfNodes(ConstJsonArray nodes, ConstJsonArray cameras, float aspect_ratio) {
    Camera result;
    for (rapidjson::SizeType i = 0; i < nodes.Size(); i++)
    {
        const rapidjson::Value &node = nodes[i];

        if (!node.HasMember("camera"))
        {
            continue;
        }

        const auto &translation_array = node["translation"].GetArray();
        result.position = vec3_from_array(translation_array);

        ConstJsonArray rotation_array = node["rotation"].GetArray();
        my_quat rotation(rotation_array);

        result.right = rotation * glm::vec3(-1.0, 0.0, 0.0);
        result.up = rotation * glm::vec3(0.0, 1.0, 0.0);
        result.forward = rotation * glm::vec3(0.0, 0.0, 1.0);

        int camera_index = node["camera"].GetInt();
        const auto &camera = cameras[camera_index];
        if (!camera.HasMember("perspective"))
        {
            throw std::runtime_error("Found not perspective camera");
        }

        const auto &camera_params = camera["perspective"];
        result.fov_y = camera_params["yfov"].GetFloat();
        result.fov_x = atan(aspect_ratio * tan(result.fov_y / 2)) * 2;

        break;
    }
    return result;
}
