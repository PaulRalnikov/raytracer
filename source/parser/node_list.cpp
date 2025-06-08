#include "node_list.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "glm_parse.hpp"

NodeList::NodeList(nlohmann::json & nodes)
{
    for (auto& node : nodes) {
        m_nodes.push_back(node);
    }
    m_parent.resize(m_nodes.size(), -1);
    for (size_t node_num = 0; node_num < m_nodes.size(); node_num++) {

        if (!m_nodes[node_num].get().contains("children")) {
            continue;
        }
        std::vector<int> children = m_nodes[node_num].get()["children"];
        for (auto child_num : children) {
            if (m_parent[child_num] != -1) {
                throw std::runtime_error("Node" + std::to_string(child_num) + "have two or more parents");
            }

            m_parent[child_num] = node_num;
        }
    }
}

static glm::mat4x4 get_self_transform(const nlohmann::json& node) {
    glm::mat4x4 result(1.f);
    if (node.contains("matrix")) {
        return mat4x4_from_array(node["matrix"]);
    }
    my_quat rotation;
    if (node.contains("rotation")) {
        rotation = my_quat_from_array(node["rotation"]);
    }

    glm::vec3 scale(1.f);
    if (node.contains("scale")) {
        scale = vec3_from_array(node["scale"]);
    }

    glm::vec3 traslation(0.f);
    if (node.contains("translation")) {
        traslation = vec3_from_array(node["translation"]);
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

std::pair<const nlohmann::json &, glm::mat4x4> NodeList::operator[](size_t index) const
{
    return {m_nodes[index], get_transform(index)};
}

size_t NodeList::size() const {
    return m_parent.size();
}
