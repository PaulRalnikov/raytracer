#include <iostream>
#include <chrono>
#include <vector>
#include <glm/glm.hpp>
#include <rapidjson/writer.h>

#include "scene.hpp"
#include "raw_pixels.hpp"

std::string path_withoud_extention(std::string path) {
    while (path.back() != '.') {
        path.pop_back();
    }
    path.pop_back();
    return path;
}

static int positive_atoi(const char* str) {
    int result = std::atoi(str);
    if (result <= 0) {
        throw std::runtime_error("Result of atoi must be greater zero");
    }
    return result;
}

int main(int argc, char *argv[])
{
    const static int DEFAULF_ARGC = 6;
    assert(argc == DEFAULF_ARGC || argc == DEFAULF_ARGC + 1);
    std::string input_path = argv[1];
    int width = positive_atoi(argv[2]);
    int height = positive_atoi(argv[3]);
    int samples = positive_atoi(argv[4]);
    std::string output_path = argv[5];

    bool png_flag = (argc == DEFAULF_ARGC + 1 && std::string(argv[argc - 1]) == "--png");

    // Scene scene = Scene::fromGltf(input_path, width, height, samples);
    Scene scene;
    scene.readTxt(input_path);

    // return 0;

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
