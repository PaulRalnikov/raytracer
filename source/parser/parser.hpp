#pragma once
#include "scene/scene.hpp"
#include "node_list.hpp"

class Parser {
public:
    static Scene parse(std::string path, int width, int height, int samples);
};
