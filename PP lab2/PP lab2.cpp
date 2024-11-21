#include <iostream>
#include <fstream>
#include <ctime>
#include <memory>
#include <vector>
#include <tuple>
#include <string>
#include "ThreadPool.h"

const int COUNT_OF_NUMBERS = 76442;

void job(std::vector<int>& arr, int start, int end, std::vector<std::tuple<int, int, int>>& completed_jobs, std::mutex& jobs_mutex);
void initArray(std::vector<int>& arr, int n);
void waitForCompletion(ThreadPool& pool, std::vector<std::tuple<int, int, int>>& completed_jobs);

int main()
{
    std::string filename = "Data.txt";
    std::ofstream os(filename);

    ThreadPool pool;

    std::cout << "Threads start work\n";

    int thread_count = pool.getTreadsCount();

    std::vector<int> arr;
    initArray(arr, COUNT_OF_NUMBERS);

    std::vector<std::tuple<int, int, int>> completed_jobs;
    std::mutex jobs_mutex; 

    if (thread_count != 0)
    {
        int end, start;
        int step = end = COUNT_OF_NUMBERS / thread_count;
        int n = COUNT_OF_NUMBERS;
        int i = 0;
        while (end < n - 1) {
            start = i * step + i;
            end = (start + step > n) ? COUNT_OF_NUMBERS - 1: start + step;
            pool.fillQueueJobs(std::bind(job, std::ref(arr), start, end, std::ref(completed_jobs), std::ref(jobs_mutex)));
            i++;
        }
    }

    waitForCompletion(pool, completed_jobs);

    {
        std::lock_guard<std::mutex> lg(jobs_mutex);
        for (auto i : completed_jobs)
        {
            os << "Thread " << std::get<0>(i) << " completed job from index " << std::get<1>(i) << " to index " << std::get<2>(i) << std::endl;
        }

        std::cout << "\nThreads ended all work and wrote information in " << filename << std::endl;
    }
}

void initArray(std::vector<int>& arr, int n)
{
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        arr.push_back(rand() % 100);
    }
}

void waitForCompletion(ThreadPool& pool, std::vector<std::tuple<int, int, int>>& completed_jobs)
{
    while (pool.getJobCount() != completed_jobs.size())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void job(std::vector<int>& arr, int start, int end, std::vector<std::tuple<int, int, int>>& completed_jobs, std::mutex& jobs_mutex)
{
    std::hash<std::thread::id> hasher;
    int thread_id = hasher(std::this_thread::get_id());

    if (!arr.empty())
    {
        for (int i = start; i <= end; i++)
        {
            arr[i] = sqrt(arr[i]);
            arr[i] *= arr[i];
        }

        {
            std::lock_guard<std::mutex> lg(jobs_mutex);
            completed_jobs.push_back(std::make_tuple(std::abs(thread_id % 10000), start, end));
        }
    }
}
