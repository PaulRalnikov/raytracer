#pragma once
#include <vector>
#include <future>
#include <mutex>
#include "../glm/vec3.hpp"
#include "scene.hpp"

struct RaytrasyngTask {
    RaytrasyngTask(Ray ray = Ray(), int depth = 0, std::promise<glm::vec3> a_color = std::promise<glm::vec3> ());

    Ray ray;
    int depth;
    std::promise<glm::vec3> color;
};

// Pool of raytrasing tasks
class TaskPool {
public:
    TaskPool(std::vector<RaytrasyngTask>&& a_tasks, Scene& a_scene, size_t threads_count = 16);
    ~TaskPool();

    void add_task(RaytrasyngTask task);
private:

    void thread_loop();

    Scene &m_scene;

    std::mutex m_mutex; //for m_tasks
    std::vector<RaytrasyngTask> m_tasks;
    std::atomic<bool> running;
    std::vector<std::thread> m_threads;

};
