#pragma once

#include <functional>
#include <queue>
#include <thread>
#include <iostream>

namespace SmallBasic {

typedef std::function<void()> Task;

class TaskQueue {
private:
    bool destructing = false;
    std::condition_variable condition;
    std::mutex queue_mutex;
    std::thread runner_thread;
    std::queue<Task> tasks;
    void runner() {
        while (!this->destructing) {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            if (this->destructing) break;
            condition.wait(lock, [this]() { return tasks.size() > 0
                || this->destructing; });

            if (tasks.size() == 0 || this->destructing) {
                continue;
            }
            auto task = tasks.front();
            tasks.pop();

            lock.unlock();
            task();
        }
    }
public:
    TaskQueue() {
        runner_thread = std::thread([this]() { this->runner(); });
    }
    ~TaskQueue() {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        this->destructing = true;
        lock.unlock();
        condition.notify_one();
        runner_thread.join();
    }
    void push(Task task) {
        std::unique_lock<std::mutex> lock(this->queue_mutex);
        tasks.push(task);
        lock.unlock();
        condition.notify_one();
    }
    size_t size() const {
        return this->tasks.size();
    }
};

}