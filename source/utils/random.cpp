#include "random.hpp"

#include <glm/glm.hpp>

typedef struct
{
    uint64_t state;
    uint64_t inc;
} pcg32_random_t;

pcg32_random_t rng;

uint32_t pcg32_random_r()
{
    uint64_t oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 6364136223846793005ULL + (rng.inc | 1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

int random_int(int l, int r) {
    assert(r >= l);
    return l + pcg32_random_r() % (r - l + 1);
}

//generates float in [0, 1]
float random_float(float l, float r) {
    float t = pcg32_random_r() / (float)UINT32_MAX;
    return l + t * (r - l);
}

glm::vec2 random_vec2(glm::vec2 l, glm::vec2 r) {
    return glm::vec2(random_float(l.x, r.x), random_float(l.y, r.y));
}

glm::vec3 random_vec3(glm::vec3 l, glm::vec3 r){
    return glm::vec3(random_float(l.x, r.x), random_float(l.y, r.y), random_float(l.z, r.z));
}

glm::vec3 random_normal_vec3() {
    glm::vec3 v;
    do {
        v.x = random_float(-1, 1);
        v.y = random_float(-1, 1);
        v.z = random_float(-1, 1);
    } while (glm::dot(v, v) >= 1);

    return glm::normalize(v);
}
