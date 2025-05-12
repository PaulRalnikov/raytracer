#include "node_list.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm_parse.hpp"

NodeList::NodeList(ConstJsonArray arr):
    m_nodes(arr)
{
    m_parent.resize(arr.Size(), -1);
    for (rapidjson::SizeType node_num = 0; node_num < arr.Size(); node_num++) {

        if (!m_nodes[node_num].HasMember("children")) {
            continue;
        }
        ConstJsonArray children = m_nodes[node_num]["children"].GetArray();
        for (rapidjson::SizeType children_idx = 0; children_idx < children.Size(); children_idx++) {
            int children_num = children[children_idx].GetInt();

            if (m_parent[children_num] != -1) {
                throw std::runtime_error("Node" + std::to_string(children_num) + "have two or more parents");
            }

            m_parent[children_num] = node_num;
        }
    }
}

static glm::mat4x4 get_self_transform(const rapidjson::Value& node) {
    glm::mat4x4 result(1.f);
    if (node.HasMember("matrix")) {
        return mat4x4_from_array(node["matrix"].GetArray());
    }
    my_quat rotation;
    if (node.HasMember("rotation")) {
        rotation = my_quat_from_array(node["rotation"].GetArray());
    }

    glm::vec3 scale(1.f);
    if (node.HasMember("scale")) {
        scale = vec3_from_array(node["scale"].GetArray());
    }

    glm::vec3 traslation(0.f);
    if (node.HasMember("translation")) {
        traslation = vec3_from_array(node["translation"].GetArray());
    }

    glm::mat4x4 traslation_mat = glm::translate(glm::mat4x4(1.f), traslation);
    glm::mat4x4 rotation_mat = glm::mat4_cast(rotation.to_glm());
    glm::mat4x4 scale_mat = glm::scale(glm::mat4x4(1.f), scale);

    return traslation_mat * rotation_mat * scale_mat;
}

glm::mat4x4 NodeList::get_transform(size_t index) const {
    glm::mat4x4 self_transform = get_self_transform(m_nodes[index]);
    if (m_parent[index] == -1) {
        return self_transform;
    }
    return get_transform(m_parent[index]) * self_transform;
}

std::pair<const rapidjson::Value &, glm::mat4x4> NodeList::operator[](size_t index) const
{
    return {m_nodes[index], get_transform(index)};
}

size_t NodeList::size() const {
    return m_parent.size();
}
