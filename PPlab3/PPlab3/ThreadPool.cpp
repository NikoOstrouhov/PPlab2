#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    m_count_of_threads = std::thread::hardware_concurrency();
    if (m_count_of_threads)
    {
        for (int i = 0; i < m_count_of_threads; i++)
        {
            #if defined _WIN32 || defined _WIN64
            m_threads.push_back((HANDLE) _beginthreadex(NULL, 0, run, (void *) this, 0, NULL));
            #elif defined __linux__
            pthread_t thread;
            int thr = pthread_create(&thread, NULL, run, (void *) this);
            if (thr == 0)
            {
                m_threads.push_back(thread);
            }
            #endif
        }
    }
}
void ThreadPool::fillQueueJobs(const std::function<void()>& job)
{
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_jobs.push(job);
    }
    m_cond.notify_one();
}
ThreadPool::~ThreadPool()
{
    {
        std::lock_guard<std::mutex> lg(m_mutex);
        m_stop = true;
    }
    m_cond.notify_all();
    
    #if defined _WIN32 || defined _WIN64
    WaitForMultipleObjects(m_count_of_threads, m_threads.data(), TRUE, INFINITE);
    for (HANDLE hThread : m_threads) 
    {
        CloseHandle(hThread);
    }
    #elif defined __linux__
    for (pthread_t pthr : m_threads)
    {
        pthread_join(pthr, NULL);
    }
    #endif
    m_threads.clear();
}

#if defined _WIN32 || defined _WIN64
unsigned __stdcall run( void * p)
#elif defined __linux__
void* run(void * p)
#endif
{
    ThreadPool* t = (ThreadPool*)p;
    if (t != NULL)
    {
        while (true)
        {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lk(t->getm_mutex());
                t->getm_cond().wait(lk, [t] {return !t->getm_jobs().empty() || t->getm_stop(); });
                if (t->getm_stop())
                {
                    break;
                }
                job = t->getm_jobs().front();
                t->getm_jobs().pop();
                t->setJobsInProgress(t->getJobsInProgress() + 1);
            }
            job();
        }
    }
    #if defined _WIN32 || defined _WIN64
    _endthreadex(0);
    return 0;
    #elif defined __linux__
    return NULL;
    #endif
}
void ThreadPool::setJobsInProgress(int completed_jobs)
{
    m_jobs_in_progress = completed_jobs;
}
std::mutex& ThreadPool::getm_mutex()
{
    return m_mutex;
}
std::condition_variable& ThreadPool::getm_cond()
{
    return m_cond;
}
std::queue<std::function<void()>>& ThreadPool::getm_jobs()
{
    return m_jobs;
}
bool ThreadPool::getm_stop()
{
    return m_stop;
}
int ThreadPool::getJobsInProgress()
{
    {
        std::lock_guard<std::mutex> lg(m_pool_jobs_mutex);
        return m_jobs_in_progress;
    }
}
unsigned int ThreadPool::getTreadsCount()
{
    return m_count_of_threads;
}