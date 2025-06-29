#include "my_glm.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <glm/glm.hpp>

my_quat::my_quat(float x, float y, float z, float w):
    vec(x, y, z), w(w) {}
my_quat::my_quat(glm::vec3 v, float w) :
    vec(v), w(w) {}

my_quat my_quat::operator*(const my_quat& q) const {
    return my_quat(w * q.vec + q.w * vec + glm::cross(vec, q.vec), w * q.w - glm::dot(vec, q.vec));
}


my_quat my_quat::inverse() const {
    return my_quat(-vec, w);
}

glm::vec3 my_quat::operator* (const glm::vec3& v) const {
    my_quat q_hat = inverse();
    my_quat res = this->operator*(my_quat(v, 0)) * q_hat;
    return res.vec;
}

glm::quat my_quat::to_glm() const {
    glm::quat q;
    q.x = vec.x;
    q.y = vec.y;
    q.z = vec.z;
    q.w = w;
    return q;
}

std::ifstream &operator>>(std::ifstream &in, glm::vec3 &vec)
{
    in >> vec.x >> vec.y >> vec.z;
    return in;
}

std::ifstream &operator>>(std::ifstream &in, my_quat &q)
{
    in >> q.vec >> q.w;
    return in;
}

std::ostream &operator<<(std::ostream &out, const glm::vec3 v) {
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

std::ostream &operator<<(std::ostream &out, const glm::vec4 v) {
    return out << v.x << ' ' << v.y << ' ' << v.z << ' ' << v.w;
}

std::ostream &operator<<(std::ostream &out, const my_quat q)
{
    return out << q.vec << ' ' << q.w;
}

float vec_max(glm::vec3 v) {
    return std::max({v.x, v.y, v.z});
}

float vec_min(glm::vec3 v)
{
    return std::min({v.x, v.y, v.z});
}

glm::vec3 pairwice_product(glm::vec3 v) {
    return glm::vec3(v.y, v.z, v.x) * glm::vec3(v.z, v.x, v.y);
}

float sum(glm::vec3 v) {
    return v.x + v.y + v.z;
}
