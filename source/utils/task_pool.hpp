#pragma once
#include <vector>
#include <future>
#include <mutex>
#include <functional>
#include <type_traits>
#include <tuple>

#include "scene.hpp"
#include "random.hpp"

struct Task {
    explicit Task(int x = 0, int y = 0);

    int x;
    int y;
    std::promise<glm::vec3> result;
};

// Pool of raytrasing tasks
class TaskPool {
public:
    TaskPool(std::vector<Task> &&a_tasks, const Scene& scene);

    ~TaskPool();
private:
    void thread_loop();

    const Scene& m_scene;
    std::vector<Task> m_tasks;

    std::mutex m_mutex; //for m_tasks and m_rnd
    std::atomic<bool> running;
    std::vector<std::thread> m_threads;
};
