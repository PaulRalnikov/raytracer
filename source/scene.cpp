#include "scene.hpp"
#include "ray.hpp"
#include <cmath>
#include "../glm/glm.hpp"
#include <functional>
#include <algorithm>

void Scene::readTxt(std::string txt_path) {
    std::ifstream in;
    in.open(txt_path);

    std::string command;
    while (in >> command)
    {
        if (command == "DIMENSIONS") {
            in >> width >> height;
        }
        else if (command == "BG_COLOR") {
            in >> background_color.x >> background_color.y >> background_color.z;
        }
        else if (command == "CAMERA_POSITION") {
            in >> camera_position.x >> camera_position.y >> camera_position.z;
        }
        else if (command == "CAMERA_RIGHT") {
            in >> camera_right.x >> camera_right.y >> camera_right.z;
        }
        else if (command == "CAMERA_UP") {
            in >> camera_up.x >> camera_up.y >> camera_up.z;
        }
        else if (command == "CAMERA_FORWARD") {
            in >> camera_forward.x >> camera_forward.y >> camera_forward.z;
        }
        else if (command == "CAMERA_FOV_X") {
            in >> fov_x;
        }
        else if (command == "NEW_PRIMITIVE") {
            primitives.push_back(Primitive());
        }
        else if (command == "PLANE") {
            primitives.back().type = PLANE;
            in >> primitives.back().geom;
            primitives.back().geom = glm::normalize(primitives.back().geom);
        }
        else if (command == "ELLIPSOID") {
            primitives.back().type = ELLIPSOID;
            in >> primitives.back().geom;
        }
        else if (command == "BOX") {
            primitives.back().type = BOX;
            in >> primitives.back().geom;
        }
        else if (command == "POSITION") {
            in >> primitives.back().position;
        }
        else if (command == "ROTATION") {
            in >> primitives.back().rotation;
        }
        else if (command == "COLOR") {
            in >> primitives.back().color;
        }
        else if (command == "METALLIC") {
            primitives.back().material = METALLIC;
        }
        else if (command == "DIELECTRIC") {
            primitives.back().material = DIELECTRIC;
        }
        else if (command == "IOR") {
            in >> primitives.back().ior;
            primitives.back().material = DIELECTRIC;
        }
        else if (command == "RAY_DEPTH")
            {
                in >> max_ray_depth;
            }
        else if (command == "AMBIENT_LIGHT") {
            in >> abmient;
        }
        else if (command == "NEW_LIGHT") {
            lights.push_back(Light());
        }
        else if (command == "LIGHT_INTENSITY") {
            in >> lights.back().intensivity;
        }
        else if (command == "LIGHT_DIRECTION") {
            in >> lights.back().geom;
            lights.back().type = DIRECTED;
        }
        else if (command == "LIGHT_POSITION") {
            in >> lights.back().geom;
            lights.back().type = POINT;
        }
        else if (command == "LIGHT_ATTENUATION") {
            in >> lights.back().attenuation;
        }
    }

    tan_fov_x_2 = tan(fov_x / 2);
    tan_fov_y_2 = tan_fov_x_2 * height / (float) width;

    fov_y = atan(height / (float)width * tan(fov_x / 2)) * 2;

    for (auto el : primitives) {
        std::cout << el << '\n';
        std::cout << "==============================" << std::endl;
    }
}


Ray Scene::ray_to_pixel(glm::vec2 pixel) {
    glm::vec3 position(
        (2 * pixel.x / width - 1) * tan_fov_x_2,
        (- 2 * pixel.y / height + 1) * tan_fov_y_2,
        1
    );
    glm::vec3 direction = glm::normalize(position.x * camera_right + position.y * camera_up + position.z * camera_forward);
    return Ray(camera_position, direction);
}

std::optional<std::pair<float, size_t>>
Scene::intersect(Ray ray, float max_distance)
{
    std::optional<std::pair<float, size_t>> intersection = {};
    for (size_t i = 0; i < primitives.size(); i++) {
        auto& primitive = primitives[i];
        std::optional<float> prim_intersect = intersect_ray_with_primitive(ray, primitive);

        if (!prim_intersect.has_value() || prim_intersect.value() > max_distance)
            continue;

        if (!intersection.has_value() || intersection.value().first > prim_intersect.value()) {
            intersection = std::make_pair(prim_intersect.value(), i);
        }
    }
    return intersection;
}

glm::vec3 Scene::raytrace(Ray ray, int depth) {
    if (depth >= max_ray_depth) {
        return glm::vec3(0);
    }
    auto intersection = intersect(ray);
    if (!intersection.has_value())
        return background_color;

    size_t i = intersection.value().second;
    Primitive &primitive = primitives[i];

    float t = intersection.value().first; //ray position
    glm::vec3 point = ray.position + ray.direction * t;

    glm::vec3 normal = primitive.get_normal(point);
    static const float SHIFT = 1e-4;

    float normal_direction_cos = glm::dot(normal, ray.direction);
    bool inside = (normal_direction_cos > 0);
    if (inside) {
        normal *= -1;
        normal_direction_cos *= -1;
    }
    glm::vec3 reflected_direction = ray.direction - 2 * normal_direction_cos * normal;
    reflected_direction = glm::normalize(reflected_direction);
    Ray reflected_ray(point + reflected_direction * SHIFT, reflected_direction);

    switch (primitive.material)
    {
        case (DIELECTRIC):
        {
            glm::vec3 reflected_color = raytrace(reflected_ray, depth + 1);

            // Snell's law
            float mu_1 = 1;
            float mu_2 = primitive.ior;

            float cos_theta_1 = -normal_direction_cos;
            if (inside) {
                std::swap(mu_1, mu_2);
            }

            float sin_theta_2 = mu_1 / mu_2 * std::sqrt(1 - cos_theta_1 * cos_theta_1);
            if (std::abs(sin_theta_2) > 1) {
                return reflected_color;
            }
            float cos_theta_2 = std::sqrt(1 - sin_theta_2 * sin_theta_2);

            glm::vec3 refracted_direction = mu_1 / mu_2 * ray.direction + (mu_1 / mu_2 * cos_theta_1 * -cos_theta_2) * normal;
            refracted_direction = glm::normalize(refracted_direction);
            Ray refracted_ray(point + refracted_direction * SHIFT, refracted_direction);

            glm::vec3 refracted_color = raytrace(refracted_ray, depth + 1);
            if (!inside)
                refracted_color *= primitive.color;

            //Shlick's approx
            float r_0 = std::pow((mu_1 - mu_2) / (mu_1 + mu_2), 2);
            float r = r_0 + (1 - r_0) * std::pow(1 - cos_theta_1, 5);

            return r * reflected_color + (1 - r) * refracted_color;
        }
        case (DIFFUSE):
        {
            glm::vec3 result = primitive.color * abmient;
            for (Light &light : lights)
            {
                glm::vec3 light_direction = light.get_direction(point);
                glm::vec3 light_color = light.get_color(point);
                float light_distance = light.get_distance(point);

                // shadow ray
                Ray shadow_ray(point + light_direction * SHIFT, light_direction);
                auto shadow_ray_intersection = intersect(shadow_ray, light_distance);
                if (shadow_ray_intersection.has_value()) {
                    continue;
                }

                float dot = glm::dot(light_direction, normal);
                if (dot >= 0)
                    result += dot * primitive.color * light_color;
            }
            return result;
        }
        case (METALLIC):
            return raytrace(reflected_ray, depth + 1) * primitive.color;
    }
}
