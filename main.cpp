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
    assert(argc == 3);
    std::string input_path = argv[1];
    std::string output_path = argv[2];

    Scene scene;
    scene.readTxt(input_path);

    auto start = std::chrono::high_resolution_clock::now();

    RawPixels raw_pixels(scene.get_pixels());

    raw_pixels.write_to_ppm(output_path);
    raw_pixels.write_to_png(path_withoud_extention(output_path) + ".png");


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time elapced: " << duration.count() << " second" << std::endl;
}
