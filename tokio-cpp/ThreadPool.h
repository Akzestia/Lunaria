
#ifndef TOKIO_CPP_H
#define TOKIO_CPP_H

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include"../MsQuic/Linux_x64/include/msquic.h"

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    void enqueueTask(std::function<bool(HQUIC, void*)> task);

private:
    void workerThread();

    std::vector<std::thread> workers;
    std::queue<std::function<bool(HQUIC, void*)>> tasks;
    std::mutex tasksMutex;
    std::condition_variable condition;
    bool stop;
};

#endif