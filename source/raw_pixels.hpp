#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

class RawPixels {
public:
    RawPixels(std::vector<std::vector<glm::vec3> >&& pixels);

    void write_to_png(std::string path) const;
    void write_to_ppm(std::string path) const;
private:
    std::vector<unsigned char> m_raw_pixels;
    size_t m_width;
    size_t m_height;
};
