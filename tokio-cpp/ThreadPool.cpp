#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for(size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueueTask(std::function<bool(HQUIC)> task) {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void ThreadPool::workerThread() {
    while(true) {
        std::function<bool(HQUIC)> task;
        {
            std::unique_lock<std::mutex> lock(tasksMutex);
            condition.wait(lock, [this]{ return stop || !tasks.empty(); });
            if(stop && tasks.empty()) return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task(nullptr);
    }
}