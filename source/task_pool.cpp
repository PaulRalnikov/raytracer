#include "task_pool.hpp"

static const int RND_KEY = 31;

RaytrasyngTask::RaytrasyngTask(Ray ray, int depth, std::promise<glm::vec3> color):
    ray(ray), depth(depth), color(std::move(color)) {}

TaskPool::TaskPool(std::vector<RaytrasyngTask> &&a_tasks, Scene &a_scene, size_t threads_count) :
                    m_scene(a_scene),
                    m_mutex(),
                    m_rnd(RND_KEY),
                    m_tasks(std::move(a_tasks)),
                    running(true),
                    m_threads(threads_count)
{
    for (size_t i = 0; i < threads_count; i++) {
        m_threads[i] = std::thread(&TaskPool::thread_loop, this);
    }
}

void TaskPool::thread_loop() {
    while (running) {
        RaytrasyngTask task;
        {
            std::lock_guard lock(m_mutex);
            if (m_tasks.empty())
            {
                continue;
            }
            size_t i = m_rnd() % m_tasks.size();
            task = std::move(m_tasks[i]);
            std::swap(m_tasks[i], m_tasks.back());
            m_tasks.pop_back();
        }

        glm::vec3 color = m_scene.raytrace(task.ray, task.depth);
        task.color.set_value(color);
        // std::cout << "tasks for ray " << task.ray << " funished; result is " << color << std::endl;
    }
}

TaskPool::~TaskPool() {

    running.store(false);
    for (auto& thread : m_threads)
    {
        thread.join();
    }
}
