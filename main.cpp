#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtx/quaternion.hpp"
#include "source/primitive.hpp"
#include "source/scene.hpp"

void write_to_output(std::string output_path, std::vector<glm::vec3> pixels, const Scene& scene) {
    std::vector<unsigned char> real_pixels(pixels.size() * 3);

    for (size_t i = 0; i < pixels.size(); i++) {
        real_pixels[i * 3 + 0] = pixels[i].x * 255;
        real_pixels[i * 3 + 1] = pixels[i].y * 255;
        real_pixels[i * 3 + 2] = pixels[i].z * 255;
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
            bool print = false;
            if (cnt++ % mod == 0) {
                print = true;
                std::cout << "ray: " << ray << std::endl;
            }
            pixels[y * scene.width + x] =  scene.raytrace(ray, print);
        }
    }

    write_to_output(output_path, pixels, scene);
}
