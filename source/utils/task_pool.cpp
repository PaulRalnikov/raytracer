#include "task_pool.hpp"

Task::Task(int x, int y): x(x), y(y) {}

TaskPool::TaskPool(std::vector<Task> &&a_tasks, const Scene& scene) :
    m_scene(scene),
    m_tasks(std::move(a_tasks)),
    m_mutex(),
    running(true),
    m_threads(std::thread::hardware_concurrency())
    // m_threads(1)
{
    for (size_t i = 0; i < m_threads.size(); i++) {
        m_threads[i] = std::thread(&TaskPool::thread_loop, this);
    }
}

TaskPool::~TaskPool() {
    running.store(false);
    for (auto& thread : m_threads) {
        thread.join();
    }
}

void TaskPool::thread_loop()
{
    pcg32_random_t rng;
    while (running) {
        Task task;
        {
            std::lock_guard lock(m_mutex);
            if (m_tasks.empty()) {
                break;
            }
            if (m_tasks.size() % 50000 == 0) {
                std::cout << m_tasks.size() << std::endl;
            }
            size_t i = random_int(0, m_tasks.size() - 1, rng);
            task = std::move(m_tasks[i]);
            std::swap(m_tasks[i], m_tasks.back());
            m_tasks.pop_back();
        }
        task.result.set_value(m_scene.get_pixel_color(task.x, task.y, rng));
    }
}
