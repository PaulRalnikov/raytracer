#include "vector_generator.hpp"
#include "../glm/glm.hpp"

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

//generates float in [0, 1]
float generate_float(float l = -1, float r = 1) {
    float t = pcg32_random_r() / (float)UINT32_MAX;
    return l + t * (r - l);
}

glm::vec3 generate_direction(glm::vec3 normal) {
    glm::vec3 v;
    do {
        v.x = generate_float();
        v.y = generate_float();
        v.z = generate_float();
    } while (glm::dot(v, v) >= 1);
    v = glm::normalize(v);
    if (glm::dot(v, normal) < 0) {
        v = -v;
    }
    return v;
}
