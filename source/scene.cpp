#include <algorithm>
#include <cmath>
#include <functional>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "distribution/cos_weighted.hpp"
#include "distribution/box.hpp"
#include "distribution/ellipsoid.hpp"
#include "primitives/primitive.hpp"
#include "scene.hpp"
#include "ray.hpp"
#include "utils/random.hpp"
#include "utils/task_pool.hpp"

void Scene::readTxt(std::string txt_path)
{
    std::ifstream in;
    in.open(txt_path);

    std::string command;
    while (in >> command)
    {
        if (command == "DIMENSIONS")
        {
            in >> m_width >> m_height;
        }
        else if (command == "BG_COLOR")
        {
            in >> m_background_color.x >> m_background_color.y >> m_background_color.z;
        }
        else if (command == "CAMERA_POSITION")
        {
            in >> m_camera_position.x >> m_camera_position.y >> m_camera_position.z;
        }
        else if (command == "CAMERA_RIGHT")
        {
            in >> m_camera_right.x >> m_camera_right.y >> m_camera_right.z;
        }
        else if (command == "CAMERA_UP")
        {
            in >> m_camera_up.x >> m_camera_up.y >> m_camera_up.z;
        }
        else if (command == "CAMERA_FORWARD")
        {
            in >> m_camera_forward.x >> m_camera_forward.y >> m_camera_forward.z;
        }
        else if (command == "CAMERA_FOV_X")
        {
            in >> m_fov_x;
        }
        else if (command == "SAMPLES")
        {
            in >> samples;
        }
        else if (command == "RAY_DEPTH")
        {
            in >> max_ray_depth;
        }
        else if (command == "NEW_PRIMITIVE")
        {
            break;
        }
    }

    m_fov_y = atan(m_height / (float)m_width * tan(m_fov_x / 2)) * 2;

    std::vector<Primitive> primitives;
    while (in >> command)
    {

        if (command == "BOX")
        {
            Box box;
            in >> box;
            primitives.emplace_back(box);
        }
        else if (command == "PLANE")
        {
            Plane plane;
            in >> plane;
            primitives.emplace_back(plane);
        }
        else if (command == "ELLIPSOID")
        {
            Ellipsoid ellipsoid;
            in >> ellipsoid;
            primitives.push_back(ellipsoid);
        }
        else if (command == "TRIANGLE")
        {
            Triangle triangle;
            in >> triangle;
            primitives.push_back(triangle);
        }
    }
    bvh = BVH(std::move(primitives));
    setup_distribution();
}

std::vector<std::vector<glm::vec3> > Scene::get_pixels() {
    size_t tasks_count = m_width * m_height;
    std::vector<Task> tasks;
    tasks.reserve(tasks_count);

    std::vector<std::vector<std::future<glm::vec3> > > futures(m_height);

    for (int y = 0; y < m_height; y++) {
        futures[y].reserve(m_width);
        for (int x = 0; x < m_width; x++) {
            tasks.push_back(Task(x, y));
            futures[y].push_back(tasks.back().result.get_future());
        }
    }

    auto pool = TaskPool(std::move(tasks), *this);

    std::vector<std::vector<glm::vec3>> pixels(m_height, std::vector<glm::vec3>(m_width));
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            pixels[y][x] = futures[y][x].get();
        }
    }
    return pixels;
}

glm::vec3 Scene::get_pixel_color(int x, int y, pcg32_random_t& rng) const
{
    glm::vec3 color(0.0);
    for (size_t i = 0; i < samples; i++)
    {
        glm::vec2 vec_0_1(0, 1);
        glm::vec2 coords = glm::vec2(x, y) + random_vec2(vec_0_1, vec_0_1, rng);
        Ray ray = ray_to_pixel(coords);
        color += raytrace(ray, rng);
    }
    color /= samples;
    return color;
}

glm::vec3 Scene::raytrace(Ray ray, pcg32_random_t &rng, int depth) const
{
    if (depth >= max_ray_depth)
    {
        return glm::vec3(0);
    }
    auto intersection = bvh.intersect(ray);
    if (!intersection.has_value())
        return m_background_color;

    const Primitive &primitive = intersection.value().second;

    float t = intersection.value().first; // ray position
    glm::vec3 point = ray.position + ray.direction * t;

    glm::vec3 normal = get_normal(primitive, point);
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

    switch (get_material_type(primitive))
    {
    case (MaterialType::DIELECTRIC):
    {
        // Snell's law
        float mu_1 = 1;
        float mu_2 = get_ior(primitive);

        float cos_theta_1 = -normal_direction_cos;
        if (inside)
        {
            std::swap(mu_1, mu_2);
        }

        float sin_theta_2 = mu_1 / mu_2 * std::sqrt(1 - std::pow(cos_theta_1, 2));
        if (std::abs(sin_theta_2) > 1)
        {
            return get_emission(primitive) + raytrace(reflected_ray, rng, depth + 1);
        }
        float cos_theta_2 = std::sqrt(1 - std::pow(sin_theta_2, 2));

        // Shlick's approx
        float r_0 = std::pow((mu_1 - mu_2) / (mu_1 + mu_2), 2);
        float r = r_0 + (1 - r_0) * std::pow(1 - cos_theta_1, 5);

        if (random_float(0, 1, rng) < r)
        {
            // choose reflection
            return get_emission(primitive) + raytrace(reflected_ray, rng, depth + 1);
        }
        // choose refraction
        glm::vec3 refracted_direction = mu_1 / mu_2 * ray.direction + (cos_theta_1 * mu_1 / mu_2 - cos_theta_2) * normal;
        refracted_direction = glm::normalize(refracted_direction);
        Ray refracted_ray(point + refracted_direction * SHIFT, refracted_direction);

        glm::vec3 refracted_color = raytrace(refracted_ray, rng, depth + 1);
        if (!inside)
        {
            refracted_color *= get_color(primitive);
        }
        return get_emission(primitive) + refracted_color;
    }
    case (MaterialType::DIFFUSE):
    {
        glm::vec3 w = mis_distribution.sample(point, normal, rng);
        float normal_w_cos = glm::dot(w, normal);
        if (normal_w_cos <= 0 || get_color(primitive) == glm::vec3(0.0))
        {
            return get_emission(primitive);
        }
        float pdf = mis_distribution.pdf(point, normal, w);
        if (pdf == 0.f)
        {
            return get_emission(primitive);
        }
        Ray random_ray = Ray(point + w * SHIFT, w);
        glm::vec3 L_in = raytrace(random_ray, rng, depth + 1);
        glm::vec3 result = get_emission(primitive) + get_color(primitive) / glm::pi<float>() * L_in * normal_w_cos / pdf;
        return result;
    }
    case (MaterialType::METALLIC):
        return get_emission(primitive) + raytrace(reflected_ray, rng, depth + 1) * get_color(primitive);
    }
    throw std::runtime_error("Unsupported type of material");
}

Ray Scene::ray_to_pixel(glm::vec2 pixel) const {
    float tan_fov_x_2 = tan(m_fov_x / 2);
    float tan_fov_y_2 = tan_fov_x_2 * m_height / (float)m_width;

    glm::vec3 position(
        (2 * pixel.x / m_width - 1) * tan_fov_x_2,
        (-2 * pixel.y / m_height + 1) * tan_fov_y_2,
        1
    );

    glm::vec3 direction = glm::normalize(
        position.x * m_camera_right +
        position.y * m_camera_up +
        position.z * m_camera_forward
    );
    return Ray(m_camera_position, direction);
}

void Scene::setup_distribution() {
    // direct light sampling
    auto dis_distribution = std::make_shared<MixDistribution>();

    bool fl = false;
    for (const auto &el : bvh)
    {
        struct Visitor {
            void operator()(const Box& box) {
                // if (box.emission != glm::vec3(0.0)) {
                //     distribution->add_distribution(std::make_shared<BoxDistribution>(box));
                //     fl = true;
                // }
            }

            void operator()(const Ellipsoid& ellipsoid) {
                // if (ellipsoid.emission != glm::vec3(0.0)) {
                //     distribution->add_distribution(std::make_shared<EllipsoidDistribution>(ellipsoid));
                //     fl = true;
                // }
            }

            void operator()(const Plane& plane) {}
            void operator()(const Triangle& triangle) {}

            std::shared_ptr<MixDistribution> distribution;
            bool& fl;
        };
        std::visit(Visitor{dis_distribution, fl}, el);
    }

    mis_distribution.add_distribution(std::make_shared<CosWeighttedDistrubution>());
    if (fl) {
        std::cout << "Added dis distribution" << std::endl;
        mis_distribution.add_distribution(dis_distribution);
    }
}
