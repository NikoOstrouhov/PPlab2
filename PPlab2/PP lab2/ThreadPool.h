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
public:
    ThreadPool();
    ~ThreadPool();
    void fillQueueJobs(const std::function<void()>& job);
    unsigned int getTreadsCount();
    int getCompletedJobsCount();
private:
    std::condition_variable m_cond;
    std::vector<std::thread> m_threads;
    unsigned int m_count_of_threads;
    int m_completed_jobs_count = 0;
    std::mutex m_mutex;
    std::queue<std::function<void()>> m_jobs;
    bool m_stop = false;
    void run();
};