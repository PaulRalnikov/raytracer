#pragma once
#include <vector>
#include <future>
#include <mutex>
#include <random>

#include "scene.hpp"
#include "random.hpp"

struct RaytrasyngTask {
    RaytrasyngTask(int x = 0, int y = 0);

    int x, y;
    std::promise<glm::vec3> color;
};

// Pool of raytrasing tasks
class TaskPool {
public:
    TaskPool(std::vector<RaytrasyngTask>&& a_tasks, Scene& a_scene);
    ~TaskPool();

    void add_task(RaytrasyngTask task);
private:

    void thread_loop();

    Scene &m_scene;

    std::mutex m_mutex; //for m_tasks and m_rnd
    std::mt19937 m_rnd;
    std::vector<RaytrasyngTask> m_tasks;
    std::atomic<bool> running;
    std::vector<std::thread> m_threads;
};
