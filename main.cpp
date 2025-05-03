#include <iostream>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>

#include "scene.hpp"
#include "raw_pixels.hpp"

std::string path_withoud_extention(std::string path) {
    while (path.back() != '.') {
        path.pop_back();
    }
    path.pop_back();
    return path;
}

int main(int argc, char *argv[])
{
    assert(argc == 3 || argc == 4);
    std::string input_path = argv[1];
    std::string output_path = argv[2];

    bool png_flag = (argc == 4 && std::string(argv[3]) == "--png");

    Scene scene;
    scene.readTxt(input_path);

    auto start = std::chrono::high_resolution_clock::now();

    RawPixels raw_pixels(scene.get_pixels());

    if (png_flag) {
        raw_pixels.write_to_png(output_path);
    } else {
        raw_pixels.write_to_ppm(output_path);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time elapced: " << duration.count() << " second" << std::endl;
}
