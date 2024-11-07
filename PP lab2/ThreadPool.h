#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>

class ThreadPool
{
    std::vector<std::thread> m_threads;
    int m_count_of_threads;
    std::mutex m_queueMutex;
    std::queue<std::function<void()>> m_jobs;
    void run();
public:
    ThreadPool();
    ~ThreadPool();
    int getTreadsCount();
    void fillQueueJobs(const std::function<void()>& job);
    void start();
};