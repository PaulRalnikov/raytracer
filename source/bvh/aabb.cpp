#include "aabb.hpp"
#include "utils/sort_float.hpp"

AABB::AABB():
    borders{
        glm::vec3(std::numeric_limits<float>::infinity()),
        glm::vec3(-std::numeric_limits<float>::infinity())
    }
{}

AABB::AABB(const Triangle &triangle): AABB() {
    extend(triangle);
}

void AABB::extend(glm::vec3 point) {
    borders[0] = glm::min(borders[0], point);
    borders[1] = glm::max(borders[1], point);
}

void AABB::extend(const AABB& aabb) {
    borders[0] = glm::min(borders[0], aabb.borders[0]);
    borders[1] = glm::max(borders[1], aabb.borders[1]);
}

void AABB::extend(const Triangle& triangle) {
    for (const glm::vec3& p : triangle.coords) {
        extend(p);
    }
}

glm::vec3 AABB::operator[](size_t idx) const {
    assert(idx < 8);
    return glm::vec3(
        borders[(idx >> 2) & 1].x,
        borders[(idx >> 1) & 1].y,
        borders[idx & 1].z
    );
}

glm::vec3 AABB::size() const {
    return borders[1] - borders[0];
}

glm::vec3 AABB::center() const {
    return borders[0] + 0.5f * (borders[1] - borders[0]);
}

float AABB::area() const {
    return sum(pairwice_product(size())) * 2;
}

std::ostream &operator<<(std::ostream &out, const AABB &aabb) {
    out << "min: " << aabb.borders[0] << '\n';
    out << "max: " << aabb.borders[1] << '\n';
    return out;
}

std::optional<float> intersect(const Ray& ray, const AABB& aabb) {
    glm::vec3 size = 0.5f * aabb.size();
    glm::vec3 center = aabb.center();

    glm::vec3 new_ray_position = ray.position - center;

    glm::vec3 t_vec1 = (size - new_ray_position) / ray.direction;
    glm::vec3 t_vec2 = (-size - new_ray_position) / ray.direction;

    sort(t_vec1.x, t_vec2.x);
    sort(t_vec1.y, t_vec2.y);
    sort(t_vec1.z, t_vec2.z);

    float t1 = vec_max(t_vec1);
    float t2 = vec_min(t_vec2);

    if (t1 > t2 || (t1 < 0 && t2 < 0)) {
        return {};
    }
    if (t1 > 0 && t2 > 0) {
        return std::min(t1, t2);
    }

    return std::max(t1, t2);
}
