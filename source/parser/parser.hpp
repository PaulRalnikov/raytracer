#pragma once
#include "scene/scene.hpp"
#include "node_list.hpp"

#include <filesystem>

class Parser {
public:
    Parser(std::filesystem::path path);

    Scene parseScene(size_t width, size_t height, size_t samples);
private:
    nlohmann::json json;
    NodeList node_list;
    ConstJsonArray materials;
    ConstJsonArray meshes;
    ConstJsonArray acessors;
    ConstJsonArray buffer_views;
    ConstJsonArray buffers;
    ConstJsonArray cameras;

    std::vector<std::vector<char>> buffers_contents;
};
