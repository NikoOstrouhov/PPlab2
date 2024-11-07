#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    m_count_of_threads = std::thread::hardware_concurrency();
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
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_jobs.push(job);
    }
    m_cond.notify_all();
}
ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_stop = true;
    }
    m_cond.notify_all();
    for (int i = 0; i < m_threads.size(); i++)
    {
        if (m_threads[i].joinable())
        {
            {
                std::lock_guard<std::mutex> lg(m_mutex);
                std::cout << "Thread id: " << m_threads[i].get_id() << " is done\n";
            }
            m_threads[i].join();
        }
    }
    m_threads.clear();
}
int ThreadPool::getTreadsCount()
{
    return m_count_of_threads;
}
void ThreadPool::run()
{
    while (true)
    {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            std::cout << "Thread id: " << std::this_thread::get_id() << std::endl;
            m_cond.wait(lk, [this] {return !m_jobs.empty() || m_stop; });
            if (m_stop)
            {
                return;
            }
            job = m_jobs.front();
            m_jobs.pop();
            std::cout << "Thread id: " << std::this_thread::get_id() << " do work" << std::endl;
        }
        job();
    }
}