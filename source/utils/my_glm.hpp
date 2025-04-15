#include <iostream>
#include <fstream>

#include <glm/vec3.hpp>

#pragma once

struct my_quat {
    glm::vec3 vec;
    float w;
    my_quat(glm::vec3 v = glm::vec3(0), float w = 1);
    my_quat(float x, float y, float z, float w);

    my_quat operator*(const my_quat& q);
    glm::vec3 operator*(const glm::vec3& v);

    my_quat inverse() const;
};

std::ostream &operator<<(std::ostream &out, const glm::vec3 v);

std::ostream &operator<<(std::ostream &out, const my_quat v);

std::ifstream &operator>>(std::ifstream &in, glm::vec3 &vec);

std::ifstream &operator>>(std::ifstream &in, my_quat &q);

float vec_max(glm::vec3 v);

float vec_min(glm::vec3 v);
