#pragma once
#include <utility>
#include <rapidjson/document.h>
#include <glm/glm.hpp>

#include "types.hpp"

class NodeList {
public:
    NodeList(ConstJsonArray arr);

    std::pair<glm::mat4x4, const rapidjson::Value&> operator[](size_t index) const;

    size_t size() const;
private:
    ConstJsonArray m_nodes;
    std::vector<int> m_parent;
};
