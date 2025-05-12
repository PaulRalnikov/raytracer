#include "raw_pixels.hpp"
#include <fstream>

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
    #define STB_IMAGE_WRITE_IMPLEMENTATION
#endif

#include "stb_image_write.h"

static glm::vec3 saturate(glm::vec3 const &color) {
    return glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0));
}

static inline glm::vec3 aces_tonemap(glm::vec3 const &x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

static inline unsigned char color_to_byte(float color) {
    return std::round(std::clamp(color * 255, (float)0.0, (float)255.0));
}

static inline glm::vec3 gamma_correction(glm::vec3 color) {
    float exp = 1 / 2.2;
    return glm::vec3(glm::pow(color.x, exp), glm::pow(color.y, exp), glm::pow(color.z, exp));
}

RawPixels::RawPixels(std::vector<std::vector<glm::vec3> >&& pixels){
    m_height = pixels.size();
    m_width = (pixels.empty() ? 0 : pixels[0].size());
    m_raw_pixels.reserve(m_height * m_width * 3);

    for (auto&& pixels_column : pixels) {
        for (glm::vec3 pixel : pixels_column) {
            pixel = gamma_correction(aces_tonemap(pixel));

            m_raw_pixels.push_back(color_to_byte(pixel.x));
            m_raw_pixels.push_back(color_to_byte(pixel.y));
            m_raw_pixels.push_back(color_to_byte(pixel.z));
        }
    }
}

void RawPixels::write_to_ppm(std::string path) const {
    std::ofstream out(path);
    out << "P6\n";
    out << m_width << ' ' << m_height << '\n';
    out << 255 << '\n';
    out.close();

    out.open(path, std::ios::binary | std::ios::app);
    out.write(reinterpret_cast<const char *>(m_raw_pixels.data()), m_raw_pixels.size());
    out.close();
}

void RawPixels::write_to_png(std::string path) const {
    stbi_write_png(path.data(), m_width, m_height, 3, m_raw_pixels.data(), m_width * 3);
}
