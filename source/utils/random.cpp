#include "random.hpp"

#include <glm/glm.hpp>

uint32_t pcg32_random_r(pcg32_random_t& rng)
{
    uint64_t oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

int random_int(int l, int r, pcg32_random_t& rng) {
    assert(r >= l);
    return l + pcg32_random_r(rng) % (r - l + 1);
}

//generates float in [0, 1]
float random_float(float l, float r, pcg32_random_t& rng) {
    float t = pcg32_random_r(rng) / (float)UINT32_MAX;
    return l + t * (r - l);
}

glm::vec2 random_vec2(glm::vec2 l, glm::vec2 r, pcg32_random_t& rng) {
    return glm::vec2(random_float(l.x, r.x, rng), random_float(l.y, r.y, rng));
}

glm::vec3 random_vec3(glm::vec3 l, glm::vec3 r, pcg32_random_t& rng){
    return glm::vec3(random_float(l.x, r.x, rng), random_float(l.y, r.y, rng), random_float(l.z, r.z, rng));
}

glm::vec3 random_normal_vec3(pcg32_random_t& rng) {
    glm::vec3 v;
    do {
        v.x = random_float(-1, 1, rng);
        v.y = random_float(-1, 1, rng);
        v.z = random_float(-1, 1, rng);
    } while (glm::dot(v, v) >= 1);

    return glm::normalize(v);
}
