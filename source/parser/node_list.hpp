#pragma once
#include <utility>
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <functional>

#include "types.hpp"

class NodeList {
public:
    NodeList() = default;
    NodeList(nlohmann::json& nodes);

    std::pair<const nlohmann::json&, glm::mat4x4> operator[](size_t index) const;

    size_t size() const;
private:
    glm::mat4x4 get_transform(size_t index) const;

    std::vector<std::reference_wrapper<nlohmann::json> > m_nodes;
    std::vector<int> m_parent;
};
