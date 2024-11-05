#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    m_count_of_threads = std::thread::hardware_concurrency();
}
ThreadPool::~ThreadPool()
{
    for (int i = 0; i < m_threads.size(); i++)
    {
        if (m_threads[i].joinable())
        {
            m_threads[i].join();
        }
    }
    m_threads.clear();
}
void ThreadPool::start()
{
    if (m_count_of_threads)
    {
        for (int i = 0; i < m_count_of_threads; i++)
        {
            m_threads.emplace_back(&ThreadPool::run, this);
        }
    }
}
void ThreadPool::fillQueueJobs(const std::function<void()>& job)
{
    m_jobs.push(job);
}
int ThreadPool::getTreadsCount()
{
    return m_count_of_threads;
}
void ThreadPool::run()
{
    if (!m_threads.empty())
    {
        while (true)
        {
            std::function<void()> job;
            {
                std::lock_guard<std::mutex> lg(m_queueMutex);
                if (!m_jobs.empty())
                {
                    job = m_jobs.front();
                    m_jobs.pop();
                }
                else
                {
                    return;
                }
            }
            job();
        }
    }
}