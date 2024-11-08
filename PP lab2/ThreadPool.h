#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include <condition_variable>

class ThreadPool
{
    std::condition_variable m_cond;
    std::vector<std::thread> m_threads;
    int m_count_of_threads;
    std::mutex m_mutex;
    std::queue<std::function<void()>> m_jobs;
    bool m_stop = false;
    void run();
public:
    ThreadPool();
    ~ThreadPool();
    int getTreadsCount();
    void fillQueueJobs(const std::function<void()>& job);
};