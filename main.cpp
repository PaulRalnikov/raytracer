#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "source/primitive.hpp"
#include "source/scene.hpp"
#include <algorithm>

unsigned char color_to_byte(float color) {
    return std::round(std::clamp(color * 255, (float)0.0, (float)255.0));
}

void write_to_output(std::string output_path, std::vector<glm::vec3> pixels, const Scene& scene) {
    std::vector<unsigned char> real_pixels(pixels.size() * 3);

    for (size_t i = 0; i < pixels.size(); i++) {
        real_pixels[i * 3 + 0] = color_to_byte(pixels[i].x);
        real_pixels[i * 3 + 1] = color_to_byte(pixels[i].y);
        real_pixels[i * 3 + 2] = color_to_byte(pixels[i].z);
    }

    std::ofstream out;

    out.open(output_path);
    out << "P6\n";
    out << scene.width << ' ' << scene.height << '\n';
    out << 255 << '\n';
    out.close();

    out.open(output_path, std::ios::binary | std::ios::app);
    out.write(reinterpret_cast<const char *>(real_pixels.data()), real_pixels.size());
    out.close();
}

int main(int argc, char *argv[])
{
    assert(argc == 3);
    std::string input_path = argv[1];
    std::string output_path = argv[2];

    Scene scene;
    scene.readTxt(input_path);

    std::cout << scene.primitives.size() << std::endl;

    std::vector<glm::vec3> pixels(scene.width * scene.height);

    int cnt = 0;
    int mod = 10000 ;

    for (int x = 0; x < scene.width; x++) {
        for (int y = 0; y < scene.height; y++) {
            Ray ray = scene.ray_to_pixel(glm::vec2(x + 0.5, y + 0.5));
            pixels[y * scene.width + x] =  scene.raytrace(ray);
        }
    }

    write_to_output(output_path, pixels, scene);
}
