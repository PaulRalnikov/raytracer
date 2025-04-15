#include "task_pool.hpp"
#include "random.hpp"

static const int RND_KEY = 31;

RaytrasyngTask::RaytrasyngTask(int x, int y):
    x(x), y(y)
{
    color = std::promise<glm::vec3>();
}

TaskPool::TaskPool(std::vector<RaytrasyngTask> &&a_tasks, Scene &a_scene) :
                    m_scene(a_scene),
                    m_mutex(),
                    m_rnd(RND_KEY),
                    m_tasks(std::move(a_tasks)),
                    running(true),
                    m_threads(std::thread::hardware_concurrency())
                    // m_threads(1)
{
    for (size_t i = 0; i < m_threads.size(); i++) {
        m_threads[i] = std::thread(&TaskPool::thread_loop, this);
    }
}

void TaskPool::thread_loop() {
    while (running) {
        RaytrasyngTask task;
        {
            std::lock_guard lock(m_mutex);
            if (m_tasks.empty()) {
                break;
            }
            // std::cout << "task count: " << m_tasks.size() << std::endl;
            size_t i = m_rnd() % m_tasks.size();
            task = std::move(m_tasks[i]);
            std::swap(m_tasks[i], m_tasks.back());
            m_tasks.pop_back();
        }

        glm::vec3 color(0.0);
        for (size_t i = 0; i < m_scene.samples; i++) {
            glm::vec2 vec_0_1(0, 1);
            glm::vec2 coords = glm::vec2(task.x, task.y) + random_vec2(vec_0_1, vec_0_1);
            Ray ray = m_scene.ray_to_pixel(coords);
            color += m_scene.raytrace(ray);
        }
        color /= m_scene.samples;

        task.color.set_value(color);
    }
}

TaskPool::~TaskPool() {

    running.store(false);
    for (auto& thread : m_threads)
    {
        thread.join();
    }
}
