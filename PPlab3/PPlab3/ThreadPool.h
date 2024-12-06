#pragma once
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <functional>
#include <condition_variable>

#if defined _WIN32 || defined _WIN64
#include <windows.h>
#elif defined __linux__
#include <pthread.h>
#endif

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	void fillQueueJobs(const std::function<void()>& job);
	unsigned int getTreadsCount();
	int getJobsInProgress();
	std::mutex& getm_mutex();
	std::condition_variable& getm_cond();
	std::queue<std::function<void()>>& getm_jobs();
	bool getm_stop();
	void setJobsInProgress(int completed_jobs);
private:
	std::condition_variable m_cond;

	#if defined _WIN32 || defined _WIN64
	std::vector<HANDLE> m_threads;
	#elif defined __linux__
	std::vector<pthread_t> m_threads;
	#endif
	
	int m_count_of_threads;
	int m_jobs_in_progress = 0;
	std::mutex m_mutex;
	std::mutex m_pool_jobs_mutex;
	std::queue<std::function<void()>> m_jobs;
	bool m_stop = false;
};

#if defined _WIN32 || defined _WIN64
unsigned __stdcall run(void* p);
#elif defined __linux__
void* run(void* p);
#endif