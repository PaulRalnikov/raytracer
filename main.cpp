#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "source/primitive.hpp"
#include "source/scene.hpp"

int main(int argc, char *argv[])
{
    assert(argc == 3);
    std::string input_path = argv[1];
    std::string output_path = argv[2];

    Scene scene;
    scene.readTxt(input_path);

    for (auto el : scene.primitives) {
        std::cout << el << '\n';
        std::cout << "===================" << std::endl;
    }
}
