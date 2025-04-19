#include <algorithm>
#include <cmath>
#include <functional>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "scene.hpp"
#include "ray.hpp"
#include "utils/random.hpp"

Ray Scene::ray_to_pixel(glm::vec2 pixel)
{
    glm::vec3 position(
        (2 * pixel.x / width - 1) * tan_fov_x_2,
        (-2 * pixel.y / height + 1) * tan_fov_y_2,
        1);
    glm::vec3 direction = glm::normalize(position.x * camera_right + position.y * camera_up + position.z * camera_forward);
    return Ray(camera_position, direction);
}

std::optional<std::pair<float, size_t>>
Scene::intersect(Ray ray, float max_distance)
{
    std::optional<std::pair<float, size_t>> intersection = {};
    for (size_t i = 0; i < primitives.size(); i++)
    {
        auto &primitive = primitives[i];
        std::optional<float> prim_intersect = intersect_ray_with_primitive(ray, primitive);

        if (!prim_intersect.has_value() || prim_intersect.value() > max_distance)
            continue;

        if (!intersection.has_value() || intersection.value().first > prim_intersect.value())
        {
            intersection = std::make_pair(prim_intersect.value(), i);
        }
    }
    return intersection;
}

glm::vec3 Scene::raytrace(Ray ray, pcg32_random_t &rng, int depth)
{
    if (depth >= max_ray_depth)
    {
        return glm::vec3(0);
    }
    auto intersection = intersect(ray);
    if (!intersection.has_value())
        return background_color;

    size_t i = intersection.value().second;
    Primitive &primitive = primitives[i];

    float t = intersection.value().first; // ray position
    glm::vec3 point = ray.position + ray.direction * t;

    glm::vec3 normal = primitive.get_normal(point);
    static const float SHIFT = 1e-4;

    float normal_direction_cos = glm::dot(normal, ray.direction);
    bool inside = (normal_direction_cos > 0);
    if (inside)
    {
        normal *= -1;
        normal_direction_cos *= -1;
    }
    glm::vec3 reflected_direction = glm::normalize(ray.direction - 2 * normal_direction_cos * normal);
    reflected_direction = glm::normalize(reflected_direction);
    Ray reflected_ray(point + reflected_direction * SHIFT, reflected_direction);

    switch (primitive.material)
    {
    case (DIELECTRIC):
    {
        // Snell's law
        float mu_1 = 1;
        float mu_2 = primitive.ior;

        float cos_theta_1 = -normal_direction_cos;
        if (inside) {
            std::swap(mu_1, mu_2);
        }

        float sin_theta_2 = mu_1 / mu_2 * std::sqrt(1 - std::pow(cos_theta_1, 2));
        if (std::abs(sin_theta_2) > 1) {
            return primitive.emission + raytrace(reflected_ray, rng, depth + 1);
        }
        float cos_theta_2 = std::sqrt(1 - std::pow(sin_theta_2, 2));

        // Shlick's approx
        float r_0 = std::pow((mu_1 - mu_2) / (mu_1 + mu_2), 2);
        float r = r_0 + (1 - r_0) * std::pow(1 - cos_theta_1, 5);

        if (random_float(0, 1, rng) < r) {
            // choose reflection
            return primitive.emission + raytrace(reflected_ray, rng, depth + 1);
        }
        // choose refraction
        glm::vec3 refracted_direction = mu_1 / mu_2 * ray.direction + (cos_theta_1 * mu_1 / mu_2 - cos_theta_2) * normal;
        refracted_direction = glm::normalize(refracted_direction);
        Ray refracted_ray(point + refracted_direction * SHIFT, refracted_direction);

        glm::vec3 refracted_color = raytrace(refracted_ray, rng, depth + 1);
        if (!inside)
        {
            refracted_color *= primitive.color;
        }
        return primitive.emission + refracted_color;
    }
    case (DIFFUSE):
    {
        glm::vec3 w = mis_distribution.sample(point, normal, rng);
        float normal_w_cos = glm::dot(w, normal);
        if (normal_w_cos <= 0 || primitive.color == glm::vec3(0.0)) {
            return primitive.emission;
        }
        float pdf = mis_distribution.pdf(point, normal, w);
        Ray random_ray = Ray(point + w * SHIFT, w);
        glm::vec3 L_in = raytrace(random_ray, rng, depth + 1);
        return primitive.emission + primitive.color / glm::pi<float>() * L_in * normal_w_cos / pdf;
    }
    case (METALLIC):
        return raytrace(reflected_ray, rng, depth + 1) * primitive.color + primitive.emission;
    }
}

void Scene::readTxt(std::string txt_path)
{
    std::ifstream in;
    in.open(txt_path);

    std::string command;
    while (in >> command)
    {
        if (command == "DIMENSIONS")
        {
            in >> width >> height;
        }
        else if (command == "BG_COLOR")
        {
            in >> background_color.x >> background_color.y >> background_color.z;
        }
        else if (command == "CAMERA_POSITION")
        {
            in >> camera_position.x >> camera_position.y >> camera_position.z;
        }
        else if (command == "CAMERA_RIGHT")
        {
            in >> camera_right.x >> camera_right.y >> camera_right.z;
        }
        else if (command == "CAMERA_UP")
        {
            in >> camera_up.x >> camera_up.y >> camera_up.z;
        }
        else if (command == "CAMERA_FORWARD")
        {
            in >> camera_forward.x >> camera_forward.y >> camera_forward.z;
        }
        else if (command == "CAMERA_FOV_X")
        {
            in >> fov_x;
        }
        else if (command == "SAMPLES")
        {
            in >> samples;
        }
        else if (command == "NEW_PRIMITIVE")
        {
            primitives.push_back(Primitive());
        }
        else if (command == "PLANE")
        {
            primitives.back().type = PLANE;
            in >> primitives.back().geom;
            primitives.back().geom = glm::normalize(primitives.back().geom);
        }
        else if (command == "ELLIPSOID")
        {
            primitives.back().type = ELLIPSOID;
            in >> primitives.back().geom;
        }
        else if (command == "BOX")
        {
            primitives.back().type = BOX;
            in >> primitives.back().geom;
        }
        else if (command == "POSITION")
        {
            in >> primitives.back().position;
        }
        else if (command == "ROTATION")
        {
            in >> primitives.back().rotation;
        }
        else if (command == "EMISSION")
        {
            in >> primitives.back().emission;
        }
        else if (command == "COLOR")
        {
            in >> primitives.back().color;
        }
        else if (command == "METALLIC")
        {
            primitives.back().material = METALLIC;
        }
        else if (command == "DIELECTRIC")
        {
            primitives.back().material = DIELECTRIC;
        }
        else if (command == "IOR")
        {
            in >> primitives.back().ior;
            primitives.back().material = DIELECTRIC;
        }
        else if (command == "RAY_DEPTH")
        {
            in >> max_ray_depth;
        }
    }

    tan_fov_x_2 = tan(fov_x / 2);
    tan_fov_y_2 = tan_fov_x_2 * height / (float)width;

    fov_y = atan(height / (float)width * tan(fov_x / 2)) * 2;

    // direct light sampling
    auto dis_distribution = std::make_shared<MixDistribution>();

    bool fl = false;
    for (const auto &el : primitives)
    {
        if (el.type != PLANE && el.emission != glm::vec3(0.0)) {
            dis_distribution->add_distribution(
                std::make_shared<PrimitiveDistribution>(el)
            );
            fl = true;
        }
        std::cout << el << '\n';
        std::cout << "==============================" << std::endl;
    }
    mis_distribution.add_distribution(std::make_shared<CosWeighttedDistrubution>());
    if (fl) {
        mis_distribution.add_distribution(dis_distribution);
    }
}
