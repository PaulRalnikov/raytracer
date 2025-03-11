#include "my_glm.hpp"
#include <iostream>
#include <fstream>
#include "../glm/vec3.hpp"
#include "../glm/glm.hpp"

my_quat::my_quat(float x, float y, float z, float w):
    vec(x, y, z), w(w) {}
my_quat::my_quat(glm::vec3 v, float w) :
    vec(v), w(w) {}

my_quat my_quat::operator*(const my_quat& q) {
    return my_quat(w * q.vec + q.w * vec + glm::cross(vec, q.vec), w * q.w - glm::dot(vec, q.vec));
}

my_quat my_quat::inverse() const {
    return my_quat(-vec, w);
}

glm::vec3 my_quat::operator* (const glm::vec3& v) {
    my_quat q_hat = inverse();
    // std::cout << "q_hat from operator* is " << q_hat << std::endl;
    my_quat res = this->operator*(my_quat(v, 0)) * q_hat;
    return res.vec;
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

std::ostream &operator<<(std::ostream &out, const glm::vec3 v)
{
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

std::ostream &operator<<(std::ostream &out, const my_quat q)
{
    return out << q.vec << ' ' << q.w;
}
